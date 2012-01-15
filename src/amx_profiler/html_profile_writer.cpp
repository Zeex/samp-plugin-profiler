// AMX profiler for SA-MP server: http://sa-mp.com
//
// Copyright (C) 2011-2012 Sergey Zolotarev
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include "function.h"
#include "function_info.h"
#include "html_profile_writer.h"
#include "performance_counter.h"

namespace amx_profiler {

void HtmlProfileWriter::Write(const std::string &script_name, std::ostream &stream,
		const std::vector<const FunctionInfo*> &stats)
{
	stream <<
	"<html>\n"
	"<head>\n"
	"	<title>" << "Profile of '" << script_name << "'</title>\n"
	"</head>\n"
	"<body>\n"
	"	<h1>\n" <<
	"		Profile of '" << script_name << "'\n"
	"	</h1>\n"
	"	<table id=\"stats\" class=\"tablesorter\" border=\"1\" width=\"100%\">\n"
	"		<thead>\n"
	"			<tr>\n"
	"				<th>Type</th>\n"
	"				<th>Name</th>\n"
	"				<th>Calls</th>\n"
	"				<th>Self Time</th>\n"
	"				<th>Total Time</th>\n"
	"			</tr>\n"
	"		</thead>\n"
	"		<tbody>\n"
	;

	TimeInterval time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		time_all += (*iterator)->total_time() - (*iterator)->child_time();
	}

	TimeInterval total_time_all = 0;
	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		total_time_all += (*iterator)->total_time();
	}

	for (std::vector<const FunctionInfo*>::const_iterator iterator = stats.begin();
			iterator != stats.end(); ++iterator) {
		stream
		<< "		<tr>\n"
		<< "			<td>" << (*iterator)->function()->type() << "</td>\n"
		<< "			<td>" << (*iterator)->function()->name() << "</td>\n"
		<< "			<td>" << (*iterator)->num_calls() << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2)
			<< static_cast<double>(((*iterator)->total_time() - (*iterator)->child_time()) * 100) / time_all << "</td>\n"
		<< "			<td>" << std::fixed << std::setprecision(2)
			<< static_cast<double>((*iterator)->total_time() * 100) / total_time_all << "</td>\n"
		<< "		</tr>\n";
	}

	stream <<
	"		</tbody>\n"
	"	</table>\n"
	;

	stream <<
	"	<br/>"
	"	<footer>\n"
	//"		Generated on " << boost::posix_time::second_clock::local_time() << "\n"
	"	</footer>\n"
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://code.jquery.com/jquery-latest.min.js\"></script>\n"
	"	<script type=\"text/javascript\"\n"
	"		src=\"http://autobahn.tablesorter.com/jquery.tablesorter.min.js\"></script>\n"
	"	<script type=\"text/javascript\">\n"
	"	$(document).ready(function() {\n"
	"		$(\"#stats\").tablesorter();\n"
	"	});\n"
	"	</script>\n"
	"</body>\n"
	"</html>\n"
	;
}

} // namespace amx_profiler
