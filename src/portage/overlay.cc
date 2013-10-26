// vim:set noet cinoptions= sw=4 ts=4:
// This file is part of the eix project and distributed under the
// terms of the GNU General Public License v2.
//
// Copyright (c)
//   Wolfgang Frisch <xororand@users.sourceforge.net>
//   Emil Beinroth <emilbeinroth@gmx.net>
//   Martin Väth <vaeth@mathematik.uni-wuerzburg.de>

#include <config.h>

#include <map>
#include <string>
#include <vector>

#include "eixTk/assert.h"
#include "eixTk/filenames.h"
#include "eixTk/likely.h"
#include "eixTk/null.h"
#include "eixTk/utils.h"
#include "portage/overlay.h"

using std::map;
using std::string;
using std::vector;

static map<string, string> *path_label_hash = NULLPTR;

void OverlayIdent::init_static() {
	eix_assert_static(path_label_hash == NULLPTR);
	path_label_hash = new map<string, string>;
}

OverlayIdent::OverlayIdent(const char *Path, const char *Label, Priority prio, bool ismain) {
	priority = prio;
	is_main = ismain;
	if(Path == NULLPTR) {
		path.clear();
		know_path = false;
	} else {
		path = Path;
		know_path = true;
	}
	if(Label == NULLPTR) {
		label.clear();
		know_label = false;
		return;
	}
	label = Label;
	know_label = true;
}

void OverlayIdent::readLabel_internal(const char *Path) {
	know_label = true;
	vector<string> lines;
	string my_path;
	if(Path == NULLPTR) {
		if(!know_path) {
			label.clear();
			return;
		}
		my_path = path;
	} else {
		my_path = Path;
	}
	eix_assert_static(path_label_hash != NULLPTR);
	map<string, string>::const_iterator f(path_label_hash->find(my_path));
	if(f != path_label_hash->end()) {
		label = f->second;
		return;
	}
	pushback_lines((my_path + "/profiles/repo_name").c_str(), &lines);
	label.clear();
	for(vector<string>::const_iterator i(lines.begin()); likely(i != lines.end()); ++i) {
		if(i->empty()) {
			continue;
		}
		label = *i;
		break;
	}
	(*path_label_hash)[my_path] = label;
}

string OverlayIdent::human_readable() const {
	if(label.empty())
		return path;
	return string("\"") + label + "\" " + path;
}

const char *RepoList::get_path(const string &label) {
	map<string, string>::iterator f(cache.find(label));
	if(likely(f != cache.end())) {
		return f->second.c_str();
	}
	if(trust_cache) {
		return NULLPTR;
	}
	for(iterator it(begin()); likely(it != end()); ++it) {
		if(unlikely(!it->know_path)) {
			continue;
		}
		it->readLabel();
		if(unlikely(!it->know_label)) {
			continue;
		}
		const string &l(it->label);
		const string &p(it->path);
		cache[l] = p;
		if(unlikely(l == label)) {
			return p.c_str();
		}
	}
	trust_cache = true;
	return NULLPTR;
}

RepoList::iterator RepoList::find_filename(const char *search, bool parent_ok, bool resolve_mask) {
	string mask;
	const char *s(search);
	if(resolve_mask) {
		mask = normalize_path(search, true, parent_ok);
		s = mask.c_str();
	}
	RepoList::iterator ret(end());
	for(iterator ov(begin()); likely(ov != end()); ++ov) {
		if(likely(ov->know_path)) {
			if(parent_ok) {
				if(unlikely(filename_starts_with(ov->path.c_str(), s, false))) {
					// Do as portage: The longest path is the return value
					if((ret == end()) || (ov->path.size() >= ret->path.size())) {
						ret = ov;
					}
				}
				continue;
			}
			if(unlikely(same_filenames(ov->path.c_str(), s, false, false))) {
				return ov;
			}
		}
	}
	return ret;
}

void RepoList::set_priority(OverlayIdent *overlay) {
	const char *path(NULLPTR);
	if(likely(overlay->know_label)) {
		path = get_path(overlay->label);
	}
	if(unlikely(path == NULLPTR)) {
		if(likely(overlay->know_path)) {
			path = overlay->path.c_str();
		} else {
			return;
		}
	}
	iterator it(find_filename(path, false, true));
	if(likely(it != end())) {
		overlay->priority = it->priority;
	}
}

void RepoList::push_back(const OverlayIdent &s, bool no_path_dupes) {
	if(likely(no_path_dupes)) {
		RepoList::iterator it(find_filename(s.path.c_str()));
		if(it != end()) {
			it->priority = s.priority;
			if((!it->know_label) && s.know_label) {
				it->setLabel(s.label);
				trust_cache = false;
			}
			return;
		}
	}
	trust_cache = false;
	super::push_back(s);
}

void RepoList::clear() {
	trust_cache = true;
	super::clear();
	cache.clear();
}
