#pragma once

#include <slint.h>
#include <slint-interpreter.h>
#include <jank/runtime/sequence_range.hpp>

namespace jank::runtime

{
  static object_ref slint_value_to_jank(slint::interpreter::Value const &v); // fwd-decl
  static slint::interpreter::Value jank_to_slint_value(object_ref const o); // fwd-decl

  static obj::persistent_hash_map_ref slint_struct_to_jank(slint::interpreter::Struct const &s)
  {
    auto v = obj::persistent_hash_map::empty();
    for(auto it = s.begin(); it != s.end(); ++it)
    {
      auto [name, value] = *it;
      v = v->assoc(make_box(std::string(name)), slint_value_to_jank(value));
    }
    return v;
  }

  static slint::interpreter::Struct jank_to_slint_struct(obj::persistent_hash_map_ref o)
  {
    slint::interpreter::Struct s;
    for(auto const kv : make_sequence_range(o))
    {
      auto k = kv->nth(make_box(0)).to_string();
      auto v = kv->nth(make_box(1));
      s.set_field(k.view(), jank_to_slint_value(v));
    }
    return s;
  }

  static object_ref slint_value_to_jank(slint::interpreter::Value const &v)
  {
    auto typ = v.type();
    switch(typ)
    {
      case slint::interpreter::ValueType::Void:
        return jank_nil;
      case slint::interpreter::ValueType::Number:
        return convert<double>::into_object(*v.to_number());
      case slint::interpreter::ValueType::String:
        return convert<char const *>::into_object(v.to_string()->data());
      case slint::interpreter::ValueType::Bool:
        return convert<bool>::into_object(*v.to_bool());
      case slint::interpreter::ValueType::Model:
        throw std::runtime_error("TODO: port value Model type");
      case slint::interpreter::ValueType::Struct:
        return slint_struct_to_jank(*v.to_struct());
      case slint::interpreter::ValueType::Brush:
        throw std::runtime_error("TODO: port value Brush type");
      case slint::interpreter::ValueType::Image:
        throw std::runtime_error("TODO: port value Image type");
      case slint::interpreter::ValueType::Other:
        return jank_nil;
    }
  }

  static slint::interpreter::Value jank_to_slint_value(object_ref const o)
  {
    return visit_object(
      [&](auto const typed_o) -> slint::interpreter::Value {
        using T = typename decltype(typed_o)::value_type;

        if constexpr(std::same_as<T, obj::nil>)
        {
          return {};
        }
        else if constexpr(std::same_as<T, obj::boolean>)
        {
          return typed_o->data;
        }
        else if constexpr(jtl::is_any_same<T, obj::integer, obj::small_integer>)
        {
          return static_cast<int>(typed_o->data);
        }
        else if constexpr(jtl::is_any_same<T, obj::real, obj::small_real>)
        {
          return static_cast<double>(typed_o->data);
        }
        else if constexpr(std::same_as<T, obj::keyword>)
        {
          return slint::SharedString(typed_o->to_string().view());
        }
        else if constexpr(std::same_as<T, obj::persistent_string>)
        {
          return slint::SharedString(typed_o->data.view());
        }
        else if constexpr(std::same_as<T, obj::persistent_hash_map>)
        {
          return jank_to_slint_struct(typed_o);
        }
        else
        {
          throw std::runtime_error{ util::format("unknown slint value conversion: {}\n",
                                                 object_type_str(typed_o.get_type())) };
        }
      },
      o);
  }

  template <>
  struct convert<slint::interpreter::Value>
  {
    static object_ref into_object(slint::interpreter::Value const &v)
    {
      return slint_value_to_jank(v);
    }

    static slint::interpreter::Value from_object(object_ref const o)
    {
      return jank_to_slint_value(o);
    }
  };

  template <>
  struct convert<slint::interpreter::Struct>
  {
    static obj::persistent_hash_map_ref into_object(slint::interpreter::Struct const &s)
    {
      return slint_struct_to_jank(s);
    }

    static slint::interpreter::Struct from_object(object_ref const o)
    {
      return slint::interpreter::Struct{};
    }
  };

  template <>
  struct convert<slint::SharedString>
  {
    static obj::persistent_string_ref into_object(slint::SharedString const &s)
    {
      return make_box(s.data());
    }

    static slint::SharedString from_object(object_ref const o)
    {
      return { o.to_string().c_str() };
    }
  };

  template <typename T>
  struct convert<slint::SharedVector<T>>
  {
    static obj::persistent_vector_ref into_object(slint::SharedVector<T> const &v)
    {
      auto o = obj::persistent_vector::empty();
      for(auto it = v.begin(); it != v.end(); ++it)
      {
        o = o->conj(convert<T>::into_object(*it));
      }
      return o;
    }

    static slint::SharedVector<T> from_object(object_ref const o)
    {
      slint::SharedVector<T> v;
      for(auto const e : make_sequence_range(o))
      {
        v.push_back(convert<T>::from_object(e));
      }
      return v;
    }
  };

  template <>
  struct convert<slint::interpreter::PropertyDescriptor>
  {
    static obj::persistent_hash_map_ref into_object(slint::interpreter::PropertyDescriptor const &v)
    {
      auto kw_name = __rt_ctx->intern_keyword("name").expect_ok();
      auto kw_type = __rt_ctx->intern_keyword("type").expect_ok();

      auto o = obj::persistent_hash_map::empty();
      o = o->assoc(kw_name, make_box(convert<slint::SharedString>::into_object(v.property_name)));
      o = o->assoc(kw_type, make_box("TODO"));
      return o;
    }
  };

  template <>
  struct convert<slint::interpreter::Diagnostic>
  {
    static obj::persistent_string_ref into_object(slint::interpreter::Diagnostic const &s)
    {
      return make_box(std::format("compile error {} on line {}:{}: {}",
                                  s.source_file.data(),
                                  s.line,
                                  s.column,
                                  s.message.data()));
    }
  };
}

namespace slint::core
{
  struct component_handle_helper
  {
    using strong_handle = slint::ComponentHandle<slint::interpreter::ComponentInstance>;
    using weak_handle = slint::ComponentWeakHandle<slint::interpreter::ComponentInstance>;

    component_handle_helper(strong_handle handle)
      : inner_(handle)
    {
    }

    component_handle_helper(weak_handle handle)
      : inner_(handle)
    {
    }

    strong_handle upgrade() const
    {
      if(auto const *strong = std::get_if<strong_handle>(&inner_))
      {
        return *strong;
      }
      else if(auto const *weak = std::get_if<weak_handle>(&inner_))
      {
        auto strong = weak->lock();
        if(!strong.has_value())
        {
          throw std::runtime_error("could not lock component");
        }
        return *strong;
      }
    }

    std::variant<strong_handle, weak_handle> inner_;
  };

  struct slint_callback
  {
    jank::runtime::object_ref f;

    void operator()()
    {
      f.call();
    }

    slint::interpreter::Value operator()(std::span<slint::interpreter::Value const> args)
    {
      auto args_o = jank::runtime::make_box<jank::runtime::obj::persistent_vector>();
      for(auto const &arg : args)
      {
        args_o = args_o->conj(jank::runtime::convert<slint::interpreter::Value>::into_object(arg));
      }

      auto result = jank::runtime::apply_to(f, args_o);
      return jank::runtime::convert<slint::interpreter::Value>::from_object(result);
    }
  };
}
