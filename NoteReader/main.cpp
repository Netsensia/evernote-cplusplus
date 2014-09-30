//
//  main.cpp
//  NoteReader
//
//  Created by Chris Moreton on 29/09/2014.
//  Copyright (c) 2014 Chris Moreton. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>

class Note {
public:
    std::string getGuid();
    void setGuid(std::string);
    std::string getContent();
    void setContent(std::string);
    std::vector<std::string> getTags();
    void setTags(std::vector<std::string>);
    long getCreated();
    void setCreated(std::string);
    bool hasTag(std::string, long);
    bool hasKeyword(std::string, long);
    bool createdOnOrAfter(std::string);
private:
    std::string guid;
    std::vector<std::string> tags;
    std::string content;
    long created;
    std::vector<std::string> words;
    std::unordered_map<std::string, bool> wordIndex;
};

class NoteCollection {
public:
    std::vector<Note> getNotes();
    void addNote(Note);
    void sortByDate();
private:
    static int sortByTime(Note, Note);
    std::vector<Note> notes;
};

class NoteStore {
public:
    void updateNote(Note);
    void deleteNote(std::string);
    NoteCollection search(std::string);
private:
    std::unordered_map<std::string, Note> noteDatabase;
};

class Util {
public:
    static std::string readNextLine(FILE*);
    static std::string getXmlString(FILE*, std::string);
    static Note makeNoteFromXml(std::string);
    static Note readXml(FILE*, std::string);
    static std::string extractStringFromXml(std::string, std::string);
    static std::vector<std::string> extractVectorFromXml(std::string, std::string);
    static time_t createTimestamp(std::string);
    static std::string trim(std::string const& str);
};

class NoteReader {
public:
    void go(char*, char*);
};

std::string Util::readNextLine(FILE* fp) {
    char* line = new char[500];
    std::fgets(line,  500, fp);

    if (line[strlen(line)-1] == '\n') {
        line[strlen(line)-1] = '\0';
    }
    return line;
}

std::string Util::getXmlString(FILE* fp, std::string tag) {
    std::string xmlString = "";
    std::string line;
    
    while (!feof(fp)) {
        line = Util::readNextLine(fp);

        xmlString += line;
        if (strcmp(line.c_str(), tag.c_str()) == 0) {
            break;
        }
    }
    
    size_t index = 0;
    while (true) {
        index = xmlString.find("&", index);
        if (index == std::string::npos) break;
        xmlString.replace(index++, 1, "&amp;");
        index ++;
    }
    
    return xmlString;
}

Note Util::readXml(FILE* fp, std::string tag) {
    std::string xmlString = Util::getXmlString(fp, tag);
    return Util::makeNoteFromXml(xmlString);
}

Note Util::makeNoteFromXml(std::string xmlString) {
    std::string guid = Util::extractStringFromXml(xmlString, "guid");
    std::string content = Util::extractStringFromXml(xmlString, "content");
    std::string created = Util::extractStringFromXml(xmlString, "created");
    std::vector<std::string> tags = Util::extractVectorFromXml(xmlString, "tag");

    Note note;
    
    note.setGuid(guid);
    note.setContent(content);
    note.setCreated(created);
    note.setTags(tags);
    
    return note;
}

time_t Util::createTimestamp(std::string dateString) {
    struct tm t;
    std::string format;
    if (dateString.find('-') != std::string::npos) {
        format = "%Y-%m-%dT%H:%M:%SZ";
    } else {
        format = "%Y%m%dT%H:%M:%SZ";
    }
    
    strptime(dateString.c_str(), format.c_str(), &t);
    time_t t2 = mktime(&t);
    return t2;
}

void Note::setTags(std::vector<std::string> tags) {
    this->tags = tags;
}

std::vector<std::string> Note::getTags() {
    return this->tags;
}

void Note::setCreated(std::string dateString) {
    this->created = Util::createTimestamp(dateString);
}

long Note::getCreated() {
    return this->created;
}

std::string Note::getGuid() {
    return this->guid;
}

void Note::setGuid(std::string guid) {
    this->guid = guid;
}

std::string Note::getContent() {
    return this->content;
}

void Note::setContent(std::string content) {
    this->content = content;
    
    std::string word = "";
    
    for (unsigned int i=0; i<content.length(); i++) {
        char c = content[i];
        if ((c>='A' && c<='Z') || (c >= '0' && c<='9') || (c>='a' && c<='z') || c=='\'') {
            if (c>='A' && c<='Z') {
                c += 32;
            }
            word += c;
        } else {
            if (word.length() > 0) {
                this->words.push_back(word);
                this->wordIndex[word] = true;
            }
            
            word = "";
        }
    }
    if (word.length() > 0) {
        this->words.push_back(word);
    }
}

void NoteStore::updateNote(Note note) {
    this->noteDatabase[note.getGuid()] = note;
};

void NoteStore::deleteNote(std::string guid) {
    this->noteDatabase.erase(guid);
};

std::string Util::extractStringFromXml(std::string xml, std::string tag) {
    std::string retString = "";
    std::string openTag = "<" + tag + ">";
    std::string closeTag = "</" + tag + ">";
    
    char* openTagPointer = strstr(xml.c_str(), openTag.c_str());
    char* closeTagPointer = strstr(xml.c_str(), closeTag.c_str());
    
    for (char* c=openTagPointer+openTag.length(); c<closeTagPointer; c++) {
        retString += c[0];
    }
    
    return retString;
}

std::vector<std::string> Util::extractVectorFromXml(std::string xml, std::string tag) {
    std::vector<std::string> retVector;
    std::string s;
    
    std::string openTag = "<" + tag + ">";
    std::string closeTag = "</" + tag + ">";
    
    char* openTagPointer = strstr(xml.c_str(), openTag.c_str());
    char* closeTagPointer = strstr(xml.c_str(), closeTag.c_str());
    
    char* c;
    
    while (openTagPointer != NULL) {
        s = "";
        
        for (c=openTagPointer+openTag.length(); c<closeTagPointer; c++) {
            s += c[0];
        }
        retVector.push_back(s);
        
        openTagPointer = strstr(closeTagPointer+1, openTag.c_str());
        closeTagPointer = strstr(closeTagPointer+1, closeTag.c_str());
    };

    return retVector;
}

NoteCollection NoteStore::search(std::string term) {
    
    std::transform(term.begin(), term.end(), term.begin(), ::tolower);

    std::string word = "";
    std::vector<std::string> words;
    
    for (unsigned int i=0; i<term.length(); i++) {
        char c = term[i];
        if (c != ' ') {
            word += c;
        } else {
            if (word.length() > 0) {
                words.push_back(word);
            }
            word = "";
        }
    }
    if (word.length() > 0) {
        words.push_back(word);
    }
    
    NoteCollection noteCollection;
    Note note;
    std::string guid;
    //std::cout<<"Searching for " + term + " in a database of " <<  this->noteDatabase.size() << " items"<<"\n";

    for ( auto it = this->noteDatabase.begin(); it != this->noteDatabase.end(); ++it ) {
        
        guid = it->first;
        note = it->second;
        
        bool matchesAll = true;
        
        for (unsigned int i=0; i<words.size(); i++) {
            std::string keyword = words[i];
            //std::cout<<keyword<<"\n";
            long wildcardAt = keyword.find_first_of('*');
            
            if (strcmp(keyword.substr(0, 4).c_str(), "tag:") == 0) {
                if (!note.hasTag(keyword.substr(4,keyword.length()-4),wildcardAt)) {
                    matchesAll = false;
                    break;
                }
            } else if (strcmp(keyword.substr(0, 8).c_str(), "created:") == 0) {
                if (!note.createdOnOrAfter(keyword.substr(8,keyword.length()-8))) {
                    matchesAll = false;
                    break;
                }
            } else {
                if (!note.hasKeyword(keyword, wildcardAt)) {
                    matchesAll = false;
                    break;
                }
            }
        }
        
        if (matchesAll) {
            noteCollection.addNote(note);
        }
    }
    
    noteCollection.sortByDate();
    return noteCollection;
    
}

void NoteCollection::addNote(Note note) {
    this->notes.push_back(note);
}

bool Note::hasTag(std::string term, long wildcardAt) {
    for (std::vector<std::string>::iterator it = this->tags.begin() ; it != this->tags.end(); ++it) {
        if (wildcardAt != std::string::npos) {
            if (*it == term) {
                return true;
            }
        } else {
            if (strcmp(it->substr(0, wildcardAt).c_str(), term.substr(0, wildcardAt).c_str()) == 0) {
                return true;
            }
        }
    }
    
    return false;
}

bool Note::hasKeyword(std::string keyword, long wildcardAt) {
    if (wildcardAt != std::string::npos) {
        return this->wordIndex.find(keyword) != this->wordIndex.end();
    }
    
    for (std::vector<std::string>::iterator it = this->words.begin() ; it != this->words.end(); ++it) {
        if (strcmp(it->substr(0, wildcardAt).c_str(), keyword.substr(0, wildcardAt).c_str()) == 0) {
            return true;
        }
    }
                
    return false;
}

bool Note::createdOnOrAfter(std::string dateString) {
    dateString += "T00:00:00Z";
    long timestamp = Util::createTimestamp(dateString);
    //std::cout << dateString << " " << timestamp << " " << this->created << "\n";

    return this->created >= timestamp;
}

bool compareNotesByTime(Note a, Note b) {
    return a.getCreated() < b.getCreated() ? -1 : 1;
}

void NoteCollection::sortByDate() {
    std::sort(this->notes.begin(), this->notes.end(), compareNotesByTime);
}

std::vector<Note> NoteCollection::getNotes() {
    return this->notes;
}

void NoteReader::go(char* input, char* output) {

    NoteStore noteStore;
    FILE *fp = fopen(input, "r");
    FILE *fpo = fopen(output, "w");

    int uniqueId = 1;
    std::string command;
    
    while (!feof(fp)) {
        command = Util::readNextLine(fp);

        if (strcmp(command.c_str(), "CREATE") == 0 || strcmp(command.c_str(), "UPDATE") == 0) {
            Note note = Util::readXml(fp, "</note>");
            if (strcmp(input, "inputload") == 0) {
                note.setGuid(std::to_string(++uniqueId));
            }
            noteStore.updateNote(note);
        }
        if (strcmp(command.c_str(), "DELETE") == 0) {
            std::string guid = Util::readNextLine(fp);
            noteStore.deleteNote(guid);
        }
        if (strcmp(command.c_str(), "SEARCH") == 0) {
            std::string term = Util::readNextLine(fp);
            NoteCollection noteCollection = noteStore.search(term);
            std::vector<Note> notes = noteCollection.getNotes();
            if (notes.size() == 0) {
                fprintf(fpo, "\n");
                std::cout << "\n";
            } else {
                std::string results = "";
                for (std::vector<Note>::iterator note = notes.begin() ; note != notes.end(); ++note) {
                    results += note->getGuid() + ",";
                }
                std::string output = results.substr(0,results.length()-1);
                fprintf(fpo, "%s\n", output.c_str());
                std::cout << output << "\n";
            }
            
        }

    }

    fclose(fpo);
    fclose(fp);
}

int main(int argc, const char * argv[]) {

    NoteReader noteReader;

    noteReader.go(
        (char*)"/Users/chris/git/evernote/tests/input2",
        (char*)"/Users/chris/git/evernote/tests/output_cpp_2"
    );

    return 0;
}
