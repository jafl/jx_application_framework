//===---------------------JSON.h --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef utility_JSON_h_
#define utility_JSON_h_

#include "lldb/Core/Stream.h"
#include "lldb/Utility/StringExtractor.h"

#include <inttypes.h>
#include <map>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "llvm/Support/Casting.h"

namespace lldb_private {

    class JSONValue
    {
    public:
        virtual void
        Write (Stream& s) = 0;
        
        typedef std::shared_ptr<JSONValue> SP;
        
        enum class Kind
        {
            String,
            Number,
            True,
            False,
            Null,
            Object,
            Array
        };
        
        JSONValue (Kind k) :
        m_kind(k)
        {}
        
        Kind
        GetKind() const
        {
            return m_kind;
        }
        
        virtual
        ~JSONValue () = default;
        
    private:
        const Kind m_kind;
    };
    
    class JSONString : public JSONValue
    {
    public:
        JSONString ();
        JSONString (const char* s);
        JSONString (const std::string& s);

        JSONString (const JSONString& s) = jdelete;
        JSONString&
        operator = (const JSONString& s) = jdelete;
        
        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONString> SP;
        
        std::string
        GetData () { return m_data; }
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::String;
        }
        
        ~JSONString() override = default;
        
    private:
        
        static std::string
        json_string_quote_metachars (const std::string&);
        
        std::string m_data;
    };

    class JSONNumber : public JSONValue
    {
    public:
        typedef std::shared_ptr<JSONNumber> SP;

        // We cretae a constructor for all integer and floating point type with using templates and
        // SFINAE to avoid having ambiguous overloads because of the implicit type promotion. If we
        // would have constructors only with int64_t, uint64_t and double types then constructing a
        // JSONNumber from an int32_t (or any other similar type) would fail to compile.

        template <typename T,
                  typename std::enable_if<std::is_integral<T>::value &&
                                          std::is_unsigned<T>::value>::type* = nullptr>
        explicit JSONNumber (T u) :
            JSONValue(JSONValue::Kind::Number),
            m_data_type(DataType::Unsigned)
        {
            m_data.m_unsigned = u;
        }

        template <typename T,
                  typename std::enable_if<std::is_integral<T>::value &&
                                          std::is_signed<T>::value>::type* = nullptr>
        explicit JSONNumber (T s) :
            JSONValue(JSONValue::Kind::Number),
            m_data_type(DataType::Signed)
        {
            m_data.m_signed = s;
        }

        template <typename T,
                  typename std::enable_if<std::is_floating_point<T>::value>::type* = nullptr>
        explicit JSONNumber (T d) :
            JSONValue(JSONValue::Kind::Number),
            m_data_type(DataType::Double)
        {
            m_data.m_double = d;
        }

        ~JSONNumber() override = default;

        JSONNumber (const JSONNumber& s) = jdelete;
        JSONNumber&
        operator = (const JSONNumber& s) = jdelete;

        void
        Write(Stream& s) override;

        uint64_t
        GetAsUnsigned() const;

        int64_t
        GetAsSigned() const;

        double
        GetAsDouble() const;

        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::Number;
        }

    private:
        enum class DataType : uint8_t
        {
            Unsigned,
            Signed,
            Double
        } m_data_type;

        union
        {
            uint64_t m_unsigned;
            int64_t  m_signed;
            double   m_double;
        } m_data;
    };

    class JSONTrue : public JSONValue
    {
    public:
        JSONTrue ();

        JSONTrue (const JSONTrue& s) = jdelete;
        JSONTrue&
        operator = (const JSONTrue& s) = jdelete;
        
        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONTrue> SP;
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::True;
        }
        
        ~JSONTrue() override = default;
    };

    class JSONFalse : public JSONValue
    {
    public:
        JSONFalse ();

        JSONFalse (const JSONFalse& s) = jdelete;
        JSONFalse&
        operator = (const JSONFalse& s) = jdelete;
        
        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONFalse> SP;
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::False;
        }
        
        ~JSONFalse() override = default;
    };

    class JSONNull : public JSONValue
    {
    public:
        JSONNull ();

        JSONNull (const JSONNull& s) = jdelete;
        JSONNull&
        operator = (const JSONNull& s) = jdelete;
        
        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONNull> SP;
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::Null;
        }
        
        ~JSONNull() override = default;
    };

    class JSONObject : public JSONValue
    {
    public:
        JSONObject ();
        
        JSONObject (const JSONObject& s) = jdelete;
        JSONObject&
        operator = (const JSONObject& s) = jdelete;

        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONObject> SP;
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::Object;
        }
        
        bool
        SetObject (const std::string& key,
                   JSONValue::SP value);
        
        JSONValue::SP
        GetObject (const std::string& key);
        
        ~JSONObject() override = default;
        
    private:
        typedef std::map<std::string, JSONValue::SP> Map;
        typedef Map::iterator Iterator;
        Map m_elements;
    };

    class JSONArray : public JSONValue
    {
    public:
        JSONArray ();
        
        JSONArray (const JSONArray& s) = jdelete;
        JSONArray&
        operator = (const JSONArray& s) = jdelete;
        
        void
        Write(Stream& s) override;
        
        typedef std::shared_ptr<JSONArray> SP;
        
        static bool classof(const JSONValue *V)
        {
            return V->GetKind() == JSONValue::Kind::Array;
        }
        
    private:
        typedef std::vector<JSONValue::SP> Vector;
        typedef Vector::iterator Iterator;
        typedef Vector::size_type Index;
        typedef Vector::size_type Size;
        
    public:
        bool
        SetObject (Index i,
                   JSONValue::SP value);
        
        bool
        AppendObject (JSONValue::SP value);
        
        JSONValue::SP
        GetObject (Index i);
        
        Size
        GetNumElements ();

        ~JSONArray() override = default;
        
        Vector m_elements;
    };

    class JSONParser : public StringExtractor
    {
    public:
        enum Token
        {
            Invalid,
            Error,
            ObjectStart,
            ObjectEnd,
            ArrayStart,
            ArrayEnd,
            Comma,
            Colon,
            String,
            Integer,
            Float,
            True,
            False,
            Null,
            EndOfFile
        };

        JSONParser (const char *cstr);

        int
        GetEscapedChar (bool &was_escaped);

        Token
        GetToken (std::string &value);

        JSONValue::SP
        ParseJSONValue ();

    protected:
        JSONValue::SP
        ParseJSONObject ();

        JSONValue::SP
        ParseJSONArray ();
    };
} // namespace lldb_private

#endif // utility_JSON_h_
