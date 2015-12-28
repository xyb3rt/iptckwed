#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>
#include <map>
#include <vector>
#include <exiv2/exiv2.hpp>

using namespace std;

struct mflag {
	bool all;
	bool any;
};

struct kwoption {
	map<string,unsigned int> m;
	vector<pair<string,bool> > v;
};

struct options {
	bool clear;
	bool list;
	bool quiet;
	struct mflag m;
	struct mflag x;
	struct kwoption add;
	struct kwoption remove;
	struct kwoption match;
	struct kwoption exclude;
};

static const char *cmd = "iptckwed";

void usage()
{
	fprintf(stderr, "usage: %s [-chilqv] [-aMmrXx KEYWORDS] [FILES]...\n", cmd);
}

void version()
{
	printf("%s %s - IPTC keyword editor\n", cmd, VERSION);
}

void parsekeywords(string kwlist, struct kwoption *kwop)
{
	size_t end, pos;

	for (end = pos = 0; end != string::npos && kwlist[pos] != '\0'; pos = end + 1) {
		end = kwlist.find_first_of(',', pos);
		if (pos < end) {
			string kw = kwlist.substr(pos, end - pos);

			if (kwop->m.find(kw) == kwop->m.end()) {
				kwop->m.insert(pair<string,unsigned int>(kw, kwop->v.size()));
				kwop->v.push_back(pair<string,bool>(kw, false));
			}
		}
	}
}

void resetkeywords(struct kwoption *kwop, struct mflag *m)
{
	m->all = true;
	m->any = false;

	for (map<string,unsigned int>::iterator t = kwop->m.begin(); t != kwop->m.end(); t++) {
		if (kwop->v[t->second].second)
			m->any = true;
		else
			m->all = false;
		kwop->v[t->second].second = false;
	}
}

int main(int argc, char *argv[])
{
	int opt;
	struct mflag m, x, dummy;
	struct options op;
	vector<string> files;
	vector<string>::const_iterator fi;

	Exiv2::LogMsg::setLevel(Exiv2::LogMsg::mute);
	op.clear = op.list = op.quiet = false;
	op.m.all = op.m.any = op.x.all = op.x.any = false;

	while ((opt = getopt(argc, argv, "a:chilM:m:qr:vX:x:")) != -1) {
		switch (opt) {
			case '?':
			case 'h':
				usage();
				exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
			case 'a':
				parsekeywords(optarg, &op.add);
				break;
			case 'c':
				op.clear = true;
				break;
			case 'i':
				for (string line; getline(cin, line); )
					files.push_back(line);
				break;
			case 'l':
				if (op.m.all || op.m.any || op.list) {
					fprintf(stderr, "%s: only one -l/-M/-m option allowed\n", cmd);
					exit(EXIT_FAILURE);
				}
				op.list = true;
				break;
			case 'M':
			case 'm':
				if (op.m.all || op.m.any || op.list) {
					fprintf(stderr, "%s: only one -l/-M/-m option allowed\n", cmd);
					exit(EXIT_FAILURE);
				}
				if (opt == 'M')
					op.m.all = true;
				else
					op.m.any = true;
				parsekeywords(optarg, &op.match);
				break;
			case 'q':
				op.quiet = true;
				break;
			case 'r':
				parsekeywords(optarg, &op.remove);
				break;
			case 'v':
				version();
				exit(EXIT_SUCCESS);
			case 'X':
			case 'x':
				if (op.x.any || op.x.all) {
					fprintf(stderr, "%s: only one -X/-x option allowed\n", cmd);
					exit(EXIT_FAILURE);
				}
				if (opt == 'X')
					op.x.all = true;
				else
					op.x.any = true;
				parsekeywords(optarg, &op.exclude);
				break;
		}
	}
	for (opt = optind; opt < argc; opt++)
		files.push_back(argv[opt]);
	if (files.empty()) {
		fprintf(stderr, "%s: no files to open\n", cmd);
		exit(EXIT_FAILURE);
	}

	for (fi = files.begin(); fi != files.end(); fi++) {
		try {
			bool write = false;
			map<string,unsigned int>::iterator mkw;
			vector<pair<string,bool> >::iterator vkw;
			Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(*fi);

			if (image.get() == 0)
				continue;
			image->readMetadata();
			Exiv2::IptcData iptcData = image->iptcData();
			Exiv2::IptcKey key("Iptc.Application2.Keywords");
			Exiv2::IptcData::iterator kw = iptcData.findKey(key);

			while (kw != iptcData.end()) {
				string tstr = kw->toString();
				if (op.clear || op.remove.m.find(tstr) != op.remove.m.end()) {
					kw = iptcData.erase(kw);
					write = true;
				} else {
					if ((mkw = op.add.m.find(tstr)) != op.add.m.end())
						op.add.v[mkw->second].second = true;
					if ((mkw = op.match.m.find(tstr)) != op.match.m.end())
						op.match.v[mkw->second].second = true;
					if ((mkw = op.exclude.m.find(tstr)) != op.exclude.m.end())
						op.exclude.v[mkw->second].second = true;
					kw++;
				}
			}
			for (vkw = op.add.v.begin(); vkw != op.add.v.end(); vkw++) {
				if (!vkw->second) {
					Exiv2::Value::AutoPtr value = Exiv2::Value::create(Exiv2::string);
					value->read(vkw->first);
					iptcData.add(key, value.get());
					write = vkw->second = true;
					if ((mkw = op.match.m.find(vkw->first)) != op.match.m.end())
						op.match.v[mkw->second].second = true;
				}
			}
			if (write) {
				image->setIptcData(iptcData);
				image->writeMetadata();
			}
			resetkeywords(&op.add, &dummy);
			resetkeywords(&op.match, &m);
			resetkeywords(&op.exclude, &x);

			if ((op.x.all && x.all) || (op.x.any && x.any))
				continue;
			if (op.list) {
				bool listed = false;
				for (kw = iptcData.findKey(key); kw != iptcData.end(); kw++) {
					const char *tstr = kw->toString().c_str();
					printf("%s%s%s", listed ? "" : fi->c_str(), listed ? "," : "\t", tstr);
					listed = true;
				}
				if (listed)
					printf("\n");
			} else if ((op.m.all && m.all) || (op.m.any && m.any) ||
			           (!op.m.all && !op.m.any && (op.x.all || op.x.any)))
			{
				printf("%s\n", fi->c_str());
			}
		}
		catch (Exiv2::AnyError &e) {
			if (!op.quiet)
				fprintf(stderr, "%s: %s\n", cmd, e.what());
		}
	}
	return EXIT_SUCCESS;
}

