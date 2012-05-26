HEADERS	*= src/sql/SqlHelper.h \
        src:sql/parser/sqlkeywords.h \
        src/sql/parser/tosqlparse.h \
    src/sql/entities/activity.h \
    src/sql/entities/todo.h \
    src/sql/models/activitiesmodel.h \
    src/sql/models/activitiesfiltermodel.h \
    src/sql/models/todomodel.h \

SOURCES	*= src/sql/SqlHelper.cpp \
        src/sql/parser/sqlkeywords.cpp \
        src/sql/parser/tosqlparse.cpp \
    src/sql/entities/activity.cpp \
    src/sql/entities/todo.cpp \
    src/sql/models/activitiesmodel.cpp \
    src/sql/models/activitiesfiltermodel.cpp \
    src/sql/models/todomodel.cpp

