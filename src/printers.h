// SA:MP Profiler plugin
//
// Copyright (c) 2011 Zeex
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

#ifndef PRINTERS_H
#define PRINTERS_H

#include <string>
#include <vector>

class Profile;

class Printer {
public:
	virtual void Print(const std::vector<Profile> &profiles) = 0;
};

class LogPrinter : public Printer {
public:
	LogPrinter(const std::string script_name);
	virtual void Print(const std::vector<Profile> &profiles);

private:
	std::string script_name_;
};

class HtmlPrinter : public Printer {
public:
	HtmlPrinter(const std::string &out_file, const std::string &title = "");
	virtual void Print(const std::vector<Profile> &profiles);

private:
	std::string out_file_;
	std::string title_;
};

#endif // !PRINTERS_H
