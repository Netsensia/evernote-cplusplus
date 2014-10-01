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
    void go(FILE*);
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
    int count = -1;
    
    for (unsigned int i=0; i<content.length(); i++) {
        char c = content[i];
        if ((c>='A' && c<='Z') || (c >= '0' && c<='9') || (c>='a' && c<='z') || c=='\'') {
            if (c>='A' && c<='Z') {
                c += 32;
            }
            word += c;
        } else {
            if (word.length() > 0) {
                count++;
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
    
    const char* openTagPointer = strstr(xml.c_str(), openTag.c_str());
    const char* closeTagPointer = strstr(xml.c_str(), closeTag.c_str());
    
    for (const char* c=openTagPointer+openTag.length(); c<closeTagPointer; c++) {
        retString += c[0];
    }
    
    return retString;
}

std::vector<std::string> Util::extractVectorFromXml(std::string xml, std::string tag) {
    std::vector<std::string> retVector;
    std::string s;
    
    std::string openTag = "<" + tag + ">";
    std::string closeTag = "</" + tag + ">";
    
    const char* openTagPointer = strstr(xml.c_str(), openTag.c_str());
    const char* closeTagPointer = strstr(xml.c_str(), closeTag.c_str());
    
    const char* c;
    
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

    for ( auto it = this->noteDatabase.begin(); it != this->noteDatabase.end(); ++it ) {
        
        note = it->second;
        
        bool matchesAll = true;
        
        for (unsigned int i=0; i<words.size(); i++) {
            std::string keyword = words[i];
            long wildcardAt = keyword.find_first_of('*');
            
            if (strcmp(keyword.substr(0, 4).c_str(), "tag:") == 0) {
                if (!note.hasTag(keyword.substr(4,keyword.length()-4),wildcardAt-4)) {
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
        if (wildcardAt == std::string::npos) {
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

    if (wildcardAt == std::string::npos) {
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

    return this->created >= timestamp;
}

bool compareNotesByTime(Note a, Note b) {
    return a.getCreated() < b.getCreated();
}

void NoteCollection::sortByDate() {
    std::sort(this->notes.begin(), this->notes.end(), compareNotesByTime);
}

std::vector<Note> NoteCollection::getNotes() {
    return this->notes;
}

void NoteReader::go(FILE* inputStream) {

    NoteStore noteStore;

    std::string command;
    
    while (!feof(inputStream)) {
        command = Util::readNextLine(inputStream);

        if (strcmp(command.c_str(), "CREATE") == 0 || strcmp(command.c_str(), "UPDATE") == 0) {
            Note note = Util::readXml(inputStream, "</note>");
            noteStore.updateNote(note);
        }
        if (strcmp(command.c_str(), "DELETE") == 0) {
            std::string guid = Util::readNextLine(inputStream);
            noteStore.deleteNote(guid);
        }
        if (strcmp(command.c_str(), "SEARCH") == 0) {
            std::string term = Util::readNextLine(inputStream);
            NoteCollection noteCollection = noteStore.search(term);
            std::vector<Note> notes = noteCollection.getNotes();
            if (notes.size() == 0) {
                std::cout << "\n";
            } else {
                std::string results = "";
                for (std::vector<Note>::iterator note = notes.begin() ; note != notes.end(); ++note) {
                    results += note->getGuid() + ",";
                }
                std::string output = results.substr(0,results.length()-1);
                std::cout << output << "\n";
            }
        }

    }
}

int main(int argc, const char * argv[]) {

    NoteReader noteReader;

    FILE* f = fopen("/Users/chris/git/evernote/tests/inputload", "r");
    noteReader.go(f);

    return 0;
}
