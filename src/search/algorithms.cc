// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>

#include <string>
#include <vector>
#include <map>

#include <portage/package.h>

using namespace std;

#include "algorithms.h"

map<string, unsigned int> FuzzyAlgorithm::levenshtein_map;
