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
    int getCreated();
    void setCreated(std::string);
private:
    std::string guid;
    std::vector<std::string> tags;
    std::string content;
    int created;
    std::vector<std::string> words;
    std::vector<std::string> wordIndex;
};

class NoteStore {
public:
    void updateNote(Note);
private:
    std::unordered_map<std::string, Note> noteDatabase;
};

class NoteCollection {

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
    strptime(dateString.c_str(), "%Y-%m-%dT%H:%M:%SZ", &t);
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

int Note::getCreated() {
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
}

void NoteStore::updateNote(Note note) {
    this->noteDatabase[note.getGuid()] = note;
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
            //                $guid = Util::readNextLine($fp);
            //                $noteStore->deleteNote($guid);
            //                break;
        }

    }

    //            case 'SEARCH':
    //                $term = Util::readNextLine($fp);
    //                $notes = $noteStore->search($term)->getNotes();
    //                if (count($notes) == 0) {
    //                    fprintf($fpo, PHP_EOL);
    //                } else {
    //                    $results = '';
    //                    foreach ($notes as $note) {
    //                        $results .= $note->getGuid() . ',';
    //                    }
    //                    fprintf($fpo, substr($results, 0, strlen($results)-1) . PHP_EOL);
    //                }
    //                break;

    fclose(fpo);
    fclose(fp);
}

int main(int argc, const char * argv[]) {

    NoteReader noteReader;

    noteReader.go(
        (char*)"/Users/chris/git/evernote/tests/input1",
        (char*)"/Users/chris/git/evernote/tests/output1_cpp"
    );

    return 0;
}
