#include "IFilter.hpp"

#include <vector>

#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include "complex/Filter/DataParameter.hpp"
#include "complex/Filter/ValueParameter.hpp"

namespace
{
template <class T>
void moveResult(complex::Result<T>& result, std::vector<complex::Error>& errors, std::vector<complex::Warning>& warnings)
{
  for(auto& warning : result.warnings())
  {
    warnings.push_back(std::move(warning));
  }
  if(!result.valid())
  {
    for(auto& error : result.errors())
    {
      errors.push_back(std::move(error));
    }
  }
}
} // namespace

namespace complex
{
Result<OutputActions> IFilter::preflight(const DataStructure& data, const Arguments& args, const MessageHandler& messageHandler) const
{
  Parameters params = parameters();

  std::vector<Error> errors;
  std::vector<Warning> warnings;

  Arguments resolvedArgs;

  for(auto&& [name, arg] : args)
  {
    if(!params.contains(name))
    {
      warnings.push_back(Warning{-1, fmt::format("Input contained \"{}\" which is not an accepted argument name", name)});
      continue;
    }
    resolvedArgs.insert(name, arg);
  }

  for(auto&& [name, parameter] : params)
  {
    if(!resolvedArgs.contains(name))
    {
      resolvedArgs.insert(name, parameter->defaultValue());
    }

    std::any constructedArg = parameter->construct(resolvedArgs);

    IParameter::AcceptedTypes acceptedTypes = parameter->acceptedTypes();

    if(std::find(acceptedTypes.cbegin(), acceptedTypes.cend(), constructedArg.type()) == acceptedTypes.cend())
    {
      throw std::invalid_argument("Invalid argument type");
    }

    switch(parameter->type())
    {
    case IParameter::Type::Value: {
      const auto* valueParameter = static_cast<ValueParameter*>(parameter.get());
      Result result = valueParameter->validate(constructedArg);
      moveResult(result, errors, warnings);
      if(!result.valid())
      {
        continue;
      }
      break;
    }
    case IParameter::Type::Data: {
      const auto* dataStructureParameter = static_cast<DataParameter*>(parameter.get());
      Result result = dataStructureParameter->validate(data, constructedArg);
      moveResult(result, errors, warnings);
      if(!result.valid())
      {
        continue;
      }
      break;
    }
    default:
      throw std::runtime_error("Invalid parameter type");
    }

    resolvedArgs.insert(name, constructedArg);
  }

  if(!errors.empty())
  {
    return {nonstd::make_unexpected(std::move(errors)), std::move(warnings)};
  }

  auto implResult = preflightImpl(data, args, messageHandler);

  for(auto&& warning : warnings)
  {
    implResult.warnings().push_back(std::move(warning));
  }

  return implResult;
}

Result<> IFilter::execute(DataStructure& data, const Arguments& args, const MessageHandler& messageHandler) const
{
  // determine required parameters

  // substitute defaults

  // resolve dependencies

  auto result = preflight(data, args, messageHandler);
  if(!result.valid())
  {
    return convertResult(std::move(result));
  }

  // apply actions

  return executeImpl(data, args, messageHandler);
}

nlohmann::json IFilter::toJson(const Arguments& args) const
{
  nlohmann::json json;
  Parameters params = parameters();
  for(auto&& [name, param] : params)
  {
    nlohmann::json parameterJson = param->toJson(args.at(name));
    json[name] = std::move(parameterJson);
  }
  return json;
}

Result<Arguments> IFilter::fromJson(const nlohmann::json& json) const
{
  Parameters params = parameters();
  Arguments args;
  std::vector<Error> errors;
  std::vector<Warning> warnings;
  for(auto&& [name, param] : params)
  {
    auto jsonResult = param->fromJson(json);
    moveResult(jsonResult, errors, warnings);
    if(!jsonResult.valid())
    {
      continue;
    }
    args.insert(name, std::move(jsonResult.value()));
  }
  if(errors.empty())
  {
    return {std::move(args), std::move(warnings)};
  }
  else
  {
    return {nonstd::make_unexpected(std::move(errors))};
  }
}
} // namespace complex
