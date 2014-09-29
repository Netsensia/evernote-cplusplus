//
//  main.cpp
//  NoteReader
//
//  Created by Chris Moreton on 29/09/2014.
//  Copyright (c) 2014 Chris Moreton. All rights reserved.
//

#include <iostream>

class Note {
    
};

class NoteCollection {
    
};

class Util {
    
};

class NoteReader {
    public:
        void go(char*, char*);
};

class NoteStore {
    
};

void NoteReader::go(char* $input, char* $output) {
    
    NoteStore noteStore;
    
    FILE *fp = fopen($input, "r");
    FILE *fpo = fopen($output, "w");
    
    int uniqueId = 1;
    char* command;
    while (!feof(fp)) {
        
        fscanf(fp, "%s\n", command);
        printf("%s\n", command);
//        switch ($command) {
//            case 'CREATE':
//            case 'UPDATE':
//                $note = Util::readXml($fp, 'note');
//                if ($input == 'inputload') {
//                    $note->setGuid(++$uniqueId);
//                }
//                $noteStore->updateNote($note);
//                break;
//            case 'DELETE':
//                $guid = Util::readNextLine($fp);
//                $noteStore->deleteNote($guid);
//                break;
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
//        }
    }
    
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
