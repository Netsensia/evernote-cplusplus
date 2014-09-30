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
};

class NoteReader {
public:
    void go(char*, char*);
};

std::string Util::readNextLine(FILE* fp) {
    char* line = new char[500];
    fscanf(fp, "%s\n", line);
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
    Note note;
    note.setContent(xmlString);
    return note;
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
