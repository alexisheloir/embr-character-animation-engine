//EMBR is a real time character animation engine.
//Copyright (C) 2010 Alexis Heloir
//
//This program is free software: you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation, version 3
//of the License.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY, without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this program.  If not, see
//<http://www.gnu.org/licenses/>.


#ifndef EMBR_LOGGERS_H
#define EMBR_LOGGERS_H

#include "logger.h"

static Logger ikLogger = Logger::getInstance("ikLogger");

static Logger schedulerLogger = Logger::getInstance("schedulerLogger");

static Logger parserLogger = Logger::getInstance("parserLogger");

static Logger modifierLogger = Logger::getInstance("gestureModifierLogger");

static Logger xmlLogger = Logger::getInstance("xmlLogger");

#endif

