########### next target ###############

SET(models_STAT_SRCS
   abstractsortmodel.cpp
   borrowermodel.cpp
   entrygroupmodel.cpp
   entrymodel.cpp
   entryselectionmodel.cpp
   entrysortmodel.cpp
   filtermodel.cpp
   groupsortmodel.cpp
   modeliterator.cpp
   modelmanager.cpp
   modeltest.cpp
   )

KDE4_ADD_LIBRARY(tellicomodels STATIC ${models_STAT_SRCS})

ADD_DEPENDENCIES(tellicomodels tellico_config)
