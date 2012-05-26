#include "activity.h"

Activity::Activity() :
    id(-1), date(QDate::currentDate()), type(""), description(""),
    estimation(0), urgent(false)
{
}
