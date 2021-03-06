// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Martin Väth <martin@mvath.de>

#include "portage/depend.h"
#include <config.h>  // IWYU pragma: keep

#include <string>

#include "eixTk/dialect.h"
#include "eixTk/likely.h"
#include "eixTk/stringutils.h"

using std::string;

bool Depend::use_depend;

const char Depend::c_depend[] = "${DEPEND}";
const char Depend::c_rdepend[] = "${RDEPEND}";

static CONSTEXPR const char the_same = '"';

static bool subst_the_same(string *in, const string& from);

static bool subst_the_same(string *in, const string& from) {
	if(from.empty()) {
		return false;
	}
	string::size_type pos(in->find(from));
	if(pos == string::npos) {
		return false;
	}
	string::size_type len(from.size());
	string::size_type next(pos + len);
	if(next < in->size()) {
		if((*in)[next] != ' ') {
			return false;
		}
	}
	if(pos > 0) {
		if((*in)[pos - 1] != ' ') {
			return false;
		}
	}
	in->erase(pos, len - 1);
	(*in)[pos] = the_same;
	return true;
}

void Depend::set(const string& depend, const string& rdepend, const string& pdepend, const string& bdepend, bool normspace) {
	m_depend = depend;
	m_rdepend = rdepend;
	m_pdepend = pdepend;
	m_bdepend = bdepend;
	if(normspace) {
		trimall(&m_depend);
		trimall(&m_rdepend);
		trimall(&m_pdepend);
		trimall(&m_bdepend);
	}
	subst_the_same(&m_depend, m_rdepend) || \
		subst_the_same(&m_rdepend, m_depend);
	obsolete = false;
}

string Depend::subst(const string& in, const string& text, bool obs) {
	string::size_type pos(in.find(the_same));
	if(pos == string::npos) {
		return in;
	}
	string ret(in);
	if(unlikely(obs) && ((pos + 1) != ret.size())) {
		ret[pos] = ' ';
		if(pos > 0) {
			ret.insert(++pos, 1, ' ');
		}
	} else if(unlikely(obs) && (pos > 0)) {
		ret[pos++] = ' ';
	} else {
		ret.erase(pos, 1);
	}
	ret.insert(pos, text);
	return ret;
}

bool Depend::operator==(const Depend& d) const {
	return ((get_depend() == d.get_depend()) &&
		(get_rdepend() == d.get_rdepend()) &&
		(get_pdepend() == d.get_pdepend()) &&
		(get_bdepend() == d.get_bdepend()));
}
