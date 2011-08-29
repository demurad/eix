// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <vaeth@mathematik.uni-wuerzburg.de>

#ifndef EIX__DATABASE_TYPES_H__
#define EIX__DATABASE_TYPES_H__ 1

// include <database/types.h> This comment satisfies check_include script

#include <config.h>

#include <sys/types.h>

class DBHeader;
class PackageReader;

namespace io {
	typedef unsigned char UChar;
	typedef size_t UNumber;

	typedef UNumber Catsize;
	typedef UNumber Versize;
	typedef UNumber Treesize;

	typedef off_t OffsetType;
	extern OffsetType counter;
}

#endif /* EIX__DATABASE_TYPES_H__ */
