#ifndef __LIB_JSON_PARSER_H__
#define __LIB_JSON_PARSER_H__
#include "stdafx.h"

#define ENABLE_JSON_COMMENT	// コメント許可

namespace lib {
	
class JsonNull;
class JsonString;
class JsonInt;
class JsonDouble;
class JsonBool;
class JsonArray;
class JsonObject;

// 要素基底
class JsonElement {
public:
	enum ELEMENT_TYPE {
		ELEMENT_TYPE_NULL,
		ELEMENT_TYPE_STRING,
		ELEMENT_TYPE_INT,
		ELEMENT_TYPE_DOUBLE,
		ELEMENT_TYPE_BOOL,
		ELEMENT_TYPE_ARRAY,
		ELEMENT_TYPE_OBJECT,
	};
	JsonElement(ELEMENT_TYPE type): type_(type), parse_error_(false){}
	virtual ~JsonElement(){}

	virtual void dump(std::string *out) = 0;
	
	void setParseError() {
		parse_error_ = true;
	}
	void resetParseError() { parse_error_ = false; }
	bool isParseError() { return parse_error_; }

	const char *copyValue(std::string *dst, const char *data);
	const char *copyString(std::string *dst, const char *data);
	const char *create(JsonElement **element, const char *data);

	ELEMENT_TYPE getType() { return type_; }
	bool isNull()	{ return type_ == ELEMENT_TYPE_NULL; }
	bool isString()	{ return type_ == ELEMENT_TYPE_STRING; }
	bool isInt()	{ return type_ == ELEMENT_TYPE_INT; }
	bool isDouble()	{ return type_ == ELEMENT_TYPE_DOUBLE; }
	bool isBool()	{ return type_ == ELEMENT_TYPE_BOOL; }
	bool isArray()	{ return type_ == ELEMENT_TYPE_ARRAY; }
	bool isObject()	{ return type_ == ELEMENT_TYPE_OBJECT; }
	
	const char *getString();
	long long getInt();
	long double getDouble();
	bool getBool();
	JsonArray *getArray();
	JsonObject *getObject();

private:
	ELEMENT_TYPE type_;
	bool parse_error_;
};

// NULL
class JsonNull : public JsonElement {
public:
	JsonNull(): JsonElement(ELEMENT_TYPE_NULL){}
	virtual ~JsonNull(){}
	
	virtual void dump(std::string *out) {
		*out += "null";
	}

protected:
};

// STRING
class JsonString : public JsonElement {
public:
	JsonString(): JsonElement(ELEMENT_TYPE_STRING){}
	JsonString(const char *val): JsonElement(ELEMENT_TYPE_STRING){}
	virtual ~JsonString(){}

	virtual void dump(std::string *out) {
		*out += '"' + value_ + '"';
	}

	void setValue(const char *val) { value_ = val; }
	const char *getValue() { return value_.c_str(); }

protected:
	std::string value_;
};

// INT
class JsonInt : public JsonElement {
public:
	JsonInt(): JsonElement(ELEMENT_TYPE_INT), value_(0){}
	JsonInt(long long val): JsonElement(ELEMENT_TYPE_INT), value_(val){}
	virtual ~JsonInt(){}
	
	virtual void dump(std::string *out) {
		*out += std::to_string(value_);
	}

	void setValue(long long val) { value_ = val; }
	long long getValue() { return value_; }

protected:
	long long value_;
};

// DOUBLE
class JsonDouble : public JsonElement {
public:
	JsonDouble(): JsonElement(ELEMENT_TYPE_DOUBLE), value_(0){}
	JsonDouble(long double val): JsonElement(ELEMENT_TYPE_DOUBLE), value_(val){}
	virtual ~JsonDouble(){}
	
	virtual void dump(std::string *out) {
		*out += std::to_string(value_);
	}

	void setValue(long double val) { value_ = val; }
	long double getValue() { return value_; }

protected:
	long double value_;
};

// BOOL
class JsonBool : public JsonElement {
public:
	JsonBool(): JsonElement(ELEMENT_TYPE_BOOL), value_(false){}
	JsonBool(bool val): JsonElement(ELEMENT_TYPE_BOOL), value_(val){}
	virtual ~JsonBool(){}
	
	virtual void dump(std::string *out) {
		if (value_) *out += "true";
		else *out += "false";
	}

	void setValue(bool val) { value_ = val; }
	bool getValue() { return value_; }

protected:
	bool value_;
};

// ARRAY
class JsonArray : public JsonElement {
	typedef std::vector<JsonElement*> ElementList;
public:
	JsonArray(): JsonElement(ELEMENT_TYPE_ARRAY){}
	virtual ~JsonArray()
	{
		for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
			SAFE_DELETE(*it);
		}
		element_list_.clear();
	}

	virtual void dump(std::string *out);
	const char *parse(const char *data);
	
	int getSize() { return element_list_.size(); }
	void addElement(JsonElement *element) { element_list_.push_back(element); }
	void removeElement(JsonElement *element) {
		int size = element_list_.size();
		int idx;
		for (idx = 0; idx < size; idx++) {
			if (element_list_[idx] == element) {
				SAFE_DELETE(element_list_[idx]);
				break;
			}
		}
		for (; idx < size-1; idx++) {
			element_list_[idx] = element_list_[idx+1];
		}
		element_list_.pop_back();
	}
	void removeElement(int index) {
		if (0 <= index && index < static_cast<int>(element_list_.size())) {
			removeElement(element_list_[index]);
		}
	}
	JsonElement *getElement(int index) { return element_list_[index]; }

	JsonElement *operator[](int index) {
		return getElement(index);
	}

protected:
	ElementList element_list_;
};

// OBJECT
class JsonObject : public JsonElement {
	typedef std::pair<std::string, JsonElement*> ElementData;
	typedef std::vector<ElementData> ElementList;
public:
	JsonObject(): JsonElement(ELEMENT_TYPE_OBJECT){}
	virtual ~JsonObject()
	{
		for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
			SAFE_DELETE(it->second);
		}
		element_list_.clear();
	}

	virtual void dump(std::string *out);
	const char *parse(const char *data);
	
	void addElement(const char *name, JsonElement *element) {
		element_list_.push_back(ElementData(name, element));
	}
	void removeElement(const char *name) {
		for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
			if (it->first == name) {
				element_list_.erase(it);
			}
		}
	}
	JsonElement *getElement(const char *name) {
		for (ElementList::iterator it = element_list_.begin(); it != element_list_.end(); ++it) {
			if (it->first == name) {
				return it->second;
			}
		}
		return NULL;
	}

	JsonElement *operator[](const char *name) {
		return getElement(name);
	}

protected:
	ElementList element_list_;
};

// READER
class JsonReader {
public:
	JsonReader(): root_element_(NULL), parse_error_(false), last_parse_pos_(NULL), error_line_no_(-1) {}
	JsonReader(const char *data): root_element_(NULL), parse_error_(false), last_parse_pos_(NULL), error_line_no_(-1) { parse(data); }
	~JsonReader() {
		SAFE_DELETE(root_element_);
	}


	JsonElement *getRootElement() { return root_element_; }
	bool isParseError() { return parse_error_; }
	const char *getLastParsePos() { return last_parse_pos_; }
	int getErrorLineNo() { return error_line_no_; }
	const char *dump(std::string *out);
	void parse(const char *data);

private:
	JsonElement *root_element_;
	bool parse_error_;
	const char *last_parse_pos_;
	int error_line_no_;
};


}
#endif	// __LIB_JSON_PARSER_H__