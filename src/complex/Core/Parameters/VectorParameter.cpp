#include "VectorParameter.hpp"

#include <fmt/core.h>

#include <nlohmann/json.hpp>

namespace
{
template <class...>
constexpr std::false_type always_false{};
}

namespace complex
{
template <class T>
VectorParameter<T>::VectorParameter(const std::string& name, const std::string& humanName, const std::string& helpText, const ValueType& defaultValue, const NamesType& names)
: VectorParameterBase(name, humanName, helpText)
, m_DefaultValue(defaultValue)
, m_Names(names)
{
  if(m_DefaultValue.size() != m_Names.size())
  {
    throw std::runtime_error("VectorParameter: size of names is not equal to required size");
  }
}

template <class T>
Uuid VectorParameter<T>::uuid() const
{
  return ParameterTraits<VectorParameter<T>>::uuid;
}

template <class T>
IParameter::AcceptedTypes VectorParameter<T>::acceptedTypes() const
{
  return {typeid(ValueType)};
}

template <class T>
nlohmann::json VectorParameter<T>::toJson(const std::any& value) const
{
  const auto* vec = std::any_cast<ValueType>(&value);
  if(vec == nullptr)
  {
    throw std::bad_any_cast();
  }

  auto jsonArray = nlohmann::json::array();
  for(T value : *vec)
  {
    jsonArray.push_back(value);
  }

  return jsonArray;
}

template <class T>
Result<std::any> VectorParameter<T>::fromJson(const nlohmann::json& json) const
{
  const std::string key = name();
  if(!json.contains(key))
  {
    return {nonstd::make_unexpected(std::vector<Error>{{-1, fmt::format("JSON does not contain key \"{}\"", key)}})};
  }
  auto jsonValue = json.at(key);
  if(!jsonValue.is_array())
  {
    return {nonstd::make_unexpected(std::vector<Error>{{-2, fmt::format("JSON value for key \"{}\" is not an array", key)}})};
  }
  ValueType vec;
  for(usize i = 0; i < jsonValue.size(); i++)
  {
    const auto& element = jsonValue[i];
    if constexpr(std::is_arithmetic_v<T>)
    {
      if(!element.is_number())
      {
        return {nonstd::make_unexpected(std::vector<Error>{{-3, fmt::format("JSON value for array index \"{}\" is not a number", i)}})};
      }
    }
    else
    {
      static_assert(always_false<T>, "JSON conversion not implemented for this type");
    }
    vec.push_back(element.get<T>());
  }
  return {vec};
}

template <class T>
IParameter::UniquePointer VectorParameter<T>::clone() const
{
  return std::make_unique<VectorParameter<T>>(name(), humanName(), helpText(), m_DefaultValue, m_Names);
}

template <class T>
std::any VectorParameter<T>::defaultValue() const
{
  return defaultVector();
}

template <class T>
const typename VectorParameter<T>::NamesType& VectorParameter<T>::names() const
{
  return m_Names;
}

template <class T>
usize VectorParameter<T>::size() const
{
  return m_DefaultValue.size();
}

template <class T>
const typename VectorParameter<T>::ValueType& VectorParameter<T>::defaultVector() const
{
  return m_DefaultValue;
}

template <class T>
Result<> VectorParameter<T>::validate(const std::any& value) const
{
  const auto* castValue = std::any_cast<ValueType>(&value);
  if(castValue == nullptr)
  {
    throw std::bad_any_cast();
  }
  return validateVector(*castValue);
}

template <class T>
Result<> VectorParameter<T>::validateVector(const ValueType& value) const
{
  return {};
}

template class COMPLEX_TEMPLATE_EXPORT VectorParameter<i8>;
template class COMPLEX_TEMPLATE_EXPORT VectorParameter<u8>;

template class COMPLEX_TEMPLATE_EXPORT VectorParameter<i16>;
template class COMPLEX_TEMPLATE_EXPORT VectorParameter<u16>;

template class COMPLEX_TEMPLATE_EXPORT VectorParameter<i32>;
template class COMPLEX_TEMPLATE_EXPORT VectorParameter<u32>;

template class COMPLEX_TEMPLATE_EXPORT VectorParameter<i64>;
template class COMPLEX_TEMPLATE_EXPORT VectorParameter<u64>;

template class COMPLEX_TEMPLATE_EXPORT VectorParameter<f32>;
template class COMPLEX_TEMPLATE_EXPORT VectorParameter<f64>;
} // namespace complex
