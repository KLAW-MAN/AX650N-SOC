/* Copyright (c) 2014-2019 WinnerHust
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * The names of its contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef INIFILE_H_
#define INIFILE_H_

#include <vector>
#include <algorithm>
#include <string>
#include <string.h>

using std::string;
using std::vector;

#define RET_OK 0
#define ERR_UNMATCHED_BRACKETS 2
#define ERR_SECTION_EMPTY 3
#define ERR_SECTION_ALREADY_EXISTS 4
#define ERR_PARSE_KEY_VALUE_FAILED 5
#define ERR_OPEN_FILE_FAILED 6
#define ERR_NO_ENOUGH_MEMORY 7
#define ERR_NOT_FOUND_KEY 8
#define ERR_NOT_FOUND_SECTION 9

namespace inifile
{
const char delim[] = "\n";
struct IniItem {
    string key;
    string value;
    string comment;
    string rightComment;
};

struct IniSection {
    typedef vector<IniItem>::iterator IniItem_it;
    IniItem_it begin() {
        return items.begin();
    }

    IniItem_it end() {
        return items.end();
    }

    string name;
    string comment;
    string rightComment;
    vector<IniItem> items;
};

class IniFile
{
 public:
    IniFile();
    ~IniFile() {
        release();
    }

 public:
    int Load(const string &fname);
    int Save();
    int SaveAs(const string &fname);
    int GetStringValue(const string &section, const string &key, string *value);
    int GetIntValue(const string &section, const string &key, int *value);
    int GetDoubleValue(const string &section, const string &key, double *value);
    int GetBoolValue(const string &section, const string &key, bool *value);
    int GetComment(const string &section, const string &key, string *comment);
    int GetRightComment(const string &section, const string &key, string *rightComment);

    void GetStringValueOrDefault(const string &section, const string &key, string *value, const string &defaultValue);
    void GetIntValueOrDefault(const string &section, const string &key, int *value, int defaultValue);
    void GetDoubleValueOrDefault(const string &section, const string &key, double *value, double defaultValue);
    void GetBoolValueOrDefault(const string &section, const string &key, bool *value, bool defaultValue);

    int GetValues(const string &section, const string &key, vector<string> *values);

    int GetSections(vector<string> *sections);
    int GetSectionNum();
    bool HasSection(const string &section);
    IniSection *getSection(const string &section = "");

    bool HasKey(const string &section, const string &key);

    int SetStringValue(const string &section, const string &key, const string &value);
    int SetIntValue(const string &section, const string &key, int value);
    int SetDoubleValue(const string &section, const string &key, double value);
    int SetBoolValue(const string &section, const string &key, bool value);
    int SetComment(const string &section, const string &key, const string &comment);
    int SetRightComment(const string &section, const string &key, const string &rightComment);

    void DeleteSection(const string &section);
    void DeleteKey(const string &section, const string &key);
    void SetCommentDelimiter(const string &delimiter);

    const string &GetErrMsg();
 private:
    int GetValues(const string &section, const string &key, vector<string> *value, vector<string> *comments);

    int setValue(const string &section, const string &key, const string &value, const string &comment = "");

    static void trimleft(string &str, char c = ' ');
    static void trimright(string &str, char c = ' ');
    static void trim(string &str);
 private:
    int UpdateSection(const string &cleanLine, const string &comment,
                      const string &rightComment, IniSection **section);
    int AddKeyValuePair(const string &cleanLine, const string &comment,
                        const string &rightComment, IniSection *section);

    void release();
    bool split(const string &str, const string &sep, string *left, string *right);
    bool IsCommentLine(const string &str);
    bool parse(const string &content, string *key, string *value);
    // for debug
    void print();

 private:
    int getValue(const string &section, const string &key, string *value);
    int getValue(const string &section, const string &key, string *value, string *comment);

    bool StringCmpIgnoreCase(const string &str1, const string &str2);
    bool StartWith(const string &str, const string &prefix);

 private:
    typedef vector<IniSection *>::iterator IniSection_it;
    vector<IniSection *> sections_vt;
    string iniFilePath;
    string commentDelimiter;
    string errMsg;
};

}  // endof namespace inifile

#endif  // INIFILE_H_

