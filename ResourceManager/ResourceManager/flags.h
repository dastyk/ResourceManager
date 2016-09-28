#ifndef CreateFlag
#define CreateFlag(name,type, count,...) \
struct name\
{\
static const type __VA_ARGS__;\
static const unsigned int NUM_FLAGS = count;\
name() : _flags(type()) {}\
name(const type& singleFlag) : _flags(singleFlag) {}\
name(const name& original) : _flags(original._flags) {}\
bool operator==(const name& rhs) const { return _flags == rhs._flags; }\
name&    operator =(const type& addValue) { this->_flags = addValue; return *this; }\
name&   operator |=(const type& addValue) { _flags |= addValue; return *this; }\
name    operator |(const type& addValue) { name  result(*this); result |= addValue; return result; }\
name&   operator &=(const type& maskValue) { _flags &= maskValue; return *this; }\
name    operator &(const type& maskValue) { name  result(*this); result &= maskValue; return result; }\
name    operator ~() { name  result(*this); result._flags = ~result._flags; return result; }\
operator type() { return _flags; }\
type _flags;\
}




#endif