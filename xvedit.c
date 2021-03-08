/*
    xv6 Editor
    CPTR 365 - Operating Systems
    Miro Manestar | March 7, 2021
*/

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

typedef struct Node {
    char *line;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct List {
    int count;
    struct Node *head;
    struct Node *end;
    char *name;
} List;

//Commands functions
void parse_input(char *buf, List *file);
void find(char *text, List *file);
void edit(char *range, char *text, List *file);
void end(char **textList, List *file);
void add(char *line_num, char *text, List *file);
void drop(char *range, List *file);
void list(char *range, List *file);
void quit(List *file);

//Primary functions
int main(int argc, char **argv);
void read_file(List *list, char *file);

//Utility functions
int confirm();
char toupper(char s);
void help(int argc, char **args);
void print_list(List *file);
int list_len(List *ls);
int* parse_range(char *range, List *file);
int get_cmd(char *cmd);
char* join_str(char **strgs);

//Counts the number of edits made to a file
int editCount;

//Supported commands and documentation
char *cmds[] = {
    "@END", "ADD<", "DROP", "EDIT", "FIND", "LIST", "QUIT", //Core commands
    "HELP" //Extra credit commands
};

char *cmd_doc[] = {
    "@END *text*\nAppend text as a new line at the end of each file.",
    "ADD< *line_num* *text*\nInsert new line containing text before *line_num*",
    "DROP *range*\nDelete lines in *range*",
    "EDIT *range* *text*\nReplace contents of *range* with text. Surround argument in "" to include everything inside",
    "FIND *text*\nDisplays the line numbers of lines containing *text*",
    "LIST *range*\nDisplays the lines within range. No paramter will show whole list",
    "QUIT\nExits the editor after confirming whether to save changes",
    "HELP *command*\nLists usage of the specified command"
};

void parse_input(char *buf, List *file) {
    //Replace whitespace with \0 to turn buffer into an 'array' of chars
    //because I'm lazy and don't feel like reallocating anything
    char *args[3];

    args[0] = buf;
    int argc = 1;
    //int isClosed = 0;
    for (int i = 0; buf[i]; i++) {
        //If whitespace found, it's a new argument unless we've already paresed two args
        //Also handle the case where command is entered without arg, which stops
        //trap exceptions
        if ( (buf[i] == ' ' && argc <= 2) || (buf[i] == '\n' && argc == 1) ) {
            buf[i] = '\0';
            args[argc] = buf + i + 1;

            //Don't increase it for the sake of empty args
            if ( !(buf[i] == '\n' && argc == 1) )
                argc++; 
        }
    }

    //Remove newline at the end of the input
    *(strchr(args[argc - 1], '\n')) = '\0';

    switch (get_cmd(args[0])) {
        case 0: end(args, file); break;
        case 1: add(args[1], args[2], file); break;
        case 2: drop(args[1], file); break;
        case 3: edit(args[1], args[2], file); break;
        case 4: find(args[1], file); break;
        case 5: list(args[1], file); break;
        case 6: quit(file); break;
        case 7: help(argc, args); break;
        default:
            printf(1, "Invalid command. Type HELP for help on commands.\n");
            break;
    }
}

//Prints out a list of lines from file that include text
void find(char *text, List *file) {
    int found_arr[list_len(file)];
    int found_count = 0;

    int i = 1;
    for (Node *ln = file->head; ln != 0; ln = ln->next) {
        for (int k = 0; ln->line[k]; k++) {
            if (ln->line[k] == text[0]) {
                int found = 1;
                for (int f = 0; text[f]; f++) {
                    if (text[f] == ln->line[k + f])
                        continue;
                    found = 0;
                }
                if (found) {
                    found_arr[found_count] = i;
                    found_count++;
                }
            }
        }
        i++;
    }

    if (found_count == 0) {
        printf(2, "%s is not found in this file.\n", text);
        return;
    }

    printf(2, "%s is found on line%s", text, found_count > 1 ? "s " : " ");
    for (int j = 0; j < found_count; j++)
        printf(2, "%d%s", found_arr[j], j + 1 == found_count ? "\n" : ", ");
}

//Will replace the lines in range with line from text
//Function sometimes causes pagefaults... need to make it more like drop()
void edit(char *range, char *text, List *file) {
    if (!range[0] || !text[0]) {
        printf(1, "Edit: Missing arguments\n");
        return;
    } else if (file->count <= 0) {
        printf(1, "Edit: File is empty\n");
        return;
    }

    int *ranges = parse_range(range, file);
    int start = ranges[0];
    int end = ranges[1];

    if (start < 1 || end > file->count) {
        printf(1, "Edit: Range out of bounds\n");
        return;
    }

    printf(2, "Edit %d %s?", end - start + 1, end - start + 1 > 1 ? "lines": "line");
    if (!confirm())
        return;

    Node *newLn = malloc(sizeof *newLn);
    newLn->line = malloc((strlen(text) + 1) * sizeof (char));
    file->count++;
    strcpy(newLn->line, text);

    int i = 1;
    Node *startNode = 0;
    for (Node *ln = file->head; ln != 0; ln = ln->next) {
        if (i == 1 && start == 1) {
            file->head = newLn;
        } else if (i == start - 1) {
            startNode = ln;
        } else if (i >= start && i <= end) {
            free(ln->line);
            free(ln);
        } else if (i == end + 1) {
            startNode->next = newLn;
            newLn->next = ln;
        }

        i++;
    }

    file->count -= end - start + 1;
    editCount++;
}

//Appends a new line to the end of the linked list
void end(char **textList, List *file) {

    //Since end is the only one who's first parameter can be separated by whitespace,
    //just join the whole thing into one string!
    char *text = join_str(&textList[1]);

    if (file->count <= 0) {
       add("1", text, file);
       return;
    }

    Node *ln = malloc(sizeof *ln);
    ln->line = malloc((strlen(text) + 1) * sizeof (char));
    strcpy(ln->line, text);
    
    file->end->next = ln;
    ln->prev = file->end;
    ln->next = 0;
    file->end = ln;

    file->count++;
    editCount++;
}

//Adds whats in *text as a new line before *line_num
void add(char *line_num, char *text, List *file) {
    if (!line_num[0] || !text[0]) {
        printf(1,"Add: Missing arguments\n");
        return;
    }

    //If file empty, just append it to the beginning
    if (!file->count) {
        Node *newln = malloc(sizeof *newln);
        newln->line = malloc((strlen(text) + 1) * sizeof (char));
        strcpy(newln->line, text);

        newln->prev = 0;
        newln->next = 0;
        file->head = newln;
        file->end = newln;
        file->count++;
        return;
    }

    int i = 1;
    int num = atoi(line_num);

    if (num < 1 || num > file->count) {
        printf(1, "Edit: Range out of bounds\n");
        return;
    }

    editCount++;
    for (Node *ln = file->head; ln != 0; ln = ln->next) {
        if (i + 1 == num || num == 1) {
            Node *newln = malloc(sizeof *newln);
            newln->line = malloc((strlen(text) + 1) * sizeof (char));
            strcpy(newln->line, text);

            //Need separate logic for inserting at the beginning
            if (num == 1) {
                newln->next = file->head;
                newln->next->prev = newln;
                file->head = newln;
            } else {
                newln->next = ln->next;
                newln->prev = ln;

                ln->next = newln;
                newln->next->prev = newln;
            }

            file->count++;
            return;
        }

        i++;
    }
}

//Inclusively drops the lines from file within range
//Currently has several memory leaks, but calling free to free the old lines
//and/or text causes a page fault...
void drop(char *range, List *file) {
    if (!range[0]) {
        printf(1,"Drop: Missing arguments\n");
        return;
    } else if (file->count <= 0) {
        printf(1, "Drop: File is empty\n");
        return;
    }

    int *ranges = parse_range(range, file);
    int start = ranges[0];
    int end = ranges[1];

    if (start < 1 || end > file->count) {
        printf(1, "Drop: Range out of bounds\n");
        return;
    }

    printf(2, "Drop %d %s?", end - start + 1, end - start + 1 > 1 ? "lines" : "line");
    if (!confirm())
        return;

    int i = 1;
    Node *endNode = 0;
    Node *startNode = 0;
    for (Node *ln = file->head; ln != 0; ln = ln->next) {
        if (i == start) {
            if (start != 1)
                startNode = ln->prev;
        }
        
        if (i == end) {
            if (end != file->count)
                endNode = ln->next;
        }

        if (i >= start && i <= end) {
            free(ln->line);
            free(ln);
        }

        i++;
    }

    startNode->next = endNode;
    endNode->prev = startNode;

    if (start == 1)
        file->head = endNode;
    if (end == file->count)
        file->end = startNode;
    
    editCount++;
    file->count -= end - start + 1;
    printf(2, "%d %s dropped (%d)\n", end - start + 1, end - start + 1 > 1 ? "lines" : "line", file->count);
}

//Lists the lines from file within the given range
void list(char *range, List *file) {
    if (file->count <= 0) {
        printf(1, "List: File is empty\n");
        return;
    }

    int *ranges = parse_range(range, file);
    int start = ranges[0];
    int end = ranges[1];



    //Print out the chosen range
    int i = 1;
    for (Node *ln = file->head; ln != 0; ln = ln->next) {
        if (i >= start && i <= end) {
            printf(2, "%s", ln->line);

            //Print out right-aligned line numbers
            int space = 80 - strlen(ln->line);
            for (int j = 0; j < space; j++)
                printf(2, " ");
            printf(2, "%d\n", i);
        }
        i++;
    }
}

//When run, will ask if you would like to save changes before quitting.
void quit(List *file) {
    //Just exit if the number of edits are zero
    if (editCount == 0)
        exit();

    printf(2, "Save changes?");
    if (confirm()) {
        int fd;
        if ( (fd = open(file->name, O_CREATE | O_WRONLY)) < 0) {
            printf(2, "Could not write to %s", file->name);
            exit();
        }

        for (Node *ln = file->head; ln != 0; ln = ln->next) {
            int len = strlen(ln->line);
            char *line = malloc( (len + 1) * sizeof *line);
            strcpy(line, ln->line);
            line[len] = '\n';
            write(fd, line, len + 1);
            free(line);
        }

        printf(2, "Changes saved\n");
        close(fd);
    } else {
        printf(2, "Changes not saved\n");
    }

    exit();
}


int main(int argc, char **argv) {
    static char inBuf[512]; //Input buffer
    List *file = malloc(sizeof *file); //File data
    file->name = malloc(strlen(argv[1]) * sizeof (char));
    strcpy(file->name, argv[1]);

    if (argc < 2) {
        printf(1, "Error: xvEdit requires one argument.\n");
        exit();
    }

    printf(2, "Welcome to the xv6 Editor!\n");
    read_file(file, argv[1]);
    
    //Begin primary loop
    while (1) {
        printf(2, "xvEdit> ");
        memset(inBuf, 0, sizeof inBuf);
        gets(inBuf, sizeof inBuf);
        parse_input(inBuf, file);
    }
}

//Breaks file down into a linked list where each node contains
//data for each new line
void read_file(List *list, char *file) {
    char fbuf[512];
    int fd; //File descriptor (Reference to opened file)

    //Open file and print error message if it cannot be opened
    if ( (fd = open(file, 0)) < 0 ) {
        printf(1, "xvEdit: cannot open %s\n", file);
        exit();
    }

    //Try to get a char array that is the exact size of the input
    int n; //Current buffer size from read call
    int totalN = 0;
    char *tempBuf = malloc(sizeof (char));
    while ( (n = read(fd, fbuf, sizeof fbuf)) > 0 ) {
        totalN += n;

        //Allocate extra space
        char *temp = tempBuf;
        tempBuf = malloc(totalN * sizeof (char));

        //Only use temp if we're past first iteration
        if (temp[0] != 0)
            memmove(tempBuf, temp, strlen(temp));

        //Move stuff from buffer and append to temp
        memmove(tempBuf + (totalN - n), fbuf, n);
        free(temp);
    }

    Node *prev_line = 0;
    int offset = 0;
    for (int i = 0; i < totalN; i++) {
        if (tempBuf[i] == '\n' || i == totalN - 1) {
            //Allocate space and store line in line node, only provide exactly as much
            //space as needed
            Node *ln = malloc(sizeof *ln);
            ln->line = malloc((i - offset + 1) * sizeof (char) );
            memset(ln->line, 0, i - offset + 1); //Clear out junk data
            memmove(ln->line, tempBuf + offset, i - offset); //Fill in line with snippet from tempBuf
            
            //Remove newline characters from the beginning of the line
            if (ln->line[0] == '\n')
                ln->line += 1;

            //Enter line node into linked list
            if (!list->head) {
                list->head = ln;
            } else {
                prev_line->next = ln;
                ln->prev = prev_line;
            }
            
            list->end = ln;
            prev_line = ln;
            list->count++;
            offset = i;
        }
    }

    free(tempBuf);
    
    if (n < 0) {
        printf(1, "xvEdit: read error\n");
        exit();
    }

    printf(2, "%d %s read from %s\n", list->count, list->count > 1 ? "lines" : "line", file);

    close(fd);
}

int confirm() {
    char buf[10];

    do {
        printf(2, " (Y/N): ");
        memset(buf, 0, sizeof buf);
        gets(buf, sizeof buf);

        buf[0] = toupper(buf[0]);

        if (buf[0] == 'Y')
            return 1;
        else if (buf[0] == 'N')
            return 0;
        
        printf(1, "xvEdit: Invalid input. Please try again.\n");
    } while (buf[0] != 'Y' && buf[0] != 'N');

    return 0;
}

//Really xv6? You couldn't include this????
char toupper(char s)
{
    if(('a' <= s) && (s <= 'z'))
        s = 'A' + (s - 'a');
    return s;
}

//Prints out documentation of how to use functions
void help(int argc, char **args) {
    if (!args[1][0]) {
        printf(2, "xvEdit Commands:\n");

        for (int i = 0; i < sizeof cmds / sizeof (char*); i++)
            printf(2, "%s  ", cmds[i]);

        printf(2, "\nDo help *command* to view information on the specific command.\n"
            "Everything after the first argument ignores whitespace.\n"
            "Where range is a parameter, you can use the start:end syntax to specify it.\n"
            "Commands can be abbreviated to 1-4 characters and are case-insensitive.\n");
    } else if (argc == 2) {
        switch (get_cmd(args[1])) {
            case 0: printf(2, "%s\n", cmd_doc[0]); break;
            case 1: printf(2, "%s\n", cmd_doc[1]); break;
            case 2: printf(2, "%s\n", cmd_doc[2]); break;
            case 3: printf(2, "%s\n", cmd_doc[3]); break;
            case 4: printf(2, "%s\n", cmd_doc[4]); break;
            case 5: printf(2, "%s\n", cmd_doc[5]); break;
            case 6: printf(2, "%s\n", cmd_doc[6]); break;
            case 7: printf(2, "%s\n", cmd_doc[7]); break;
            default: printf(2, "xvedit: Invalid argument\n"); break;
        }
    } else {
        printf(2, "help: Only takes a max of 1 argument\n");
    }
}

//Debug function for printing out a linked list
void print_list(List *file) {
    printf(2, "\n\n");
    for (Node *ln = file->head; ln != 0; ln = ln->next)
        printf(2, "%s", ln->line);
    printf(2, "\n");
}

//Returns the length of a linked list, assuming first item is called head
int list_len(List *ls) {
    int count = 0;
    for (Node *ln = ls->head; ln != 0; ln = ln->next)
        count++;
    return count;
}

/*
    Determines the range from a start:end string format, returns an array of 2 ints
    Examples:   ":"     returns { 1, ListLength }
                "2:"    returns { 2, ListLength }
                ":5"    returns { 1, 5 }
                "2:5"   returns { 2, 5 }
                "10"    returns { 10, 10 }
*/
int* parse_range(char *range, List *file) {
    static int ranges[2];
    ranges[0] = -1;
    ranges[1] = -1;

    int start = 1;
    int end = list_len(file);

    //If *range is empty, just return the full range
    if (range[0] == '\0' || range[0] == '\n') {
        ranges[0] = start;
        ranges[1] = end;
        return ranges;
    }

    *(strchr(range, '\n')) = '\0';
    //Replace ':' with '\0' to split the string into two
    char* tempAddr = strchr(range, ':');
    if (!tempAddr) {
        ranges[0] = atoi(range);
        ranges[1] = atoi(range);
        return ranges;
    }

    *tempAddr = '\0';
    char* range2 = tempAddr + 1;

    //Assign range if it'l :s not just a ':'
    if (range[0] != '\0')
        start = atoi(range);
    if (range2[0] != '\0')
        end = atoi(range2);

    ranges[0] = start;
    ranges[1] = end;

    if (ranges[0] == 0 || ranges[1] == 0) {
        printf(1, "Invalid range. Please enter a range in end:start format.\n");
        ranges[0] = -1;
        ranges[1] = -1;
    }


    return ranges;
}

//Matches the user input with a command, from 1 letter to the full string
//Ignores case
int get_cmd(char *cmd) {
    for (int i = 0; i < sizeof cmds / sizeof (char*); i++) {
        if (toupper(cmd[0]) == toupper(cmds[i][0])) {
            int found = 1;
            for (int k = 0; cmd[k]; k++) {
                if (toupper(cmd[k]) == toupper(cmds[i][k]))
                    continue;
                found = 0;
            }

            if (found)
                return i;
        }
    }

    return -1;
}

//Takes an array of strings and joins them into one
char* join_str(char **strgs) {
    int strLength = 0;
    for (int i = 0; strgs[i]; i++) {
        strLength += strlen(strgs[i]);
        strLength++; //For whitespace
    }
    
    char *str = malloc(strLength * sizeof (char*));

    int offset = 0;
    for (int i = 0; strgs[i]; i++) {
        strcpy(str + offset, strgs[i]);
        offset += strlen(strgs[i]) + 1;
        str[offset - 1] = ' ';
    }

    str[offset] = '\0';

    return str;

}