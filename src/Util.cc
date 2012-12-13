#include <json_spirit/json_spirit.h>
#include "Util.h"

double Util::to_d(mValue value, double default_value) {
    if (value.type() == null_type) {
        return default_value;
    } else if (value.type() == int_type) {
        return (double)value.get_int();
    } else {
        return value.get_real();
    }
}


/*
void __cdecl boost::throw_exception(class std::exception const &)
{

}
*/