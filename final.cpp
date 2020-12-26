/* Data Structures Mini Project
 * Huffman Coding Algorithm for Text Compression */

#include <bits/stdc++.h>

using namespace std;

#define MAX_TREE_HT 50

// Some global definitions
map<char, string> code;
int current_bit = 0, padding = 0;
char bit_buffer;
string bit_string = "";

struct Node {
  unsigned freq;
  char item;
  struct Node *left, *right;
};

struct MinHeap {
  unsigned size;
  unsigned capacity;
  struct Node **array;
};

// Create new Huffman tree node
struct Node *newNode(char item, unsigned freq) {
  struct Node *temp = (struct Node *)malloc(sizeof(struct Node));

  temp->left = temp->right = NULL;
  temp->item = item;
  temp->freq = freq;

  return temp;
}

// Create min heap using given capacity
struct MinHeap *createMinHeap(unsigned capacity) {
  struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));
  minHeap->size = 0;
  minHeap->capacity = capacity;
  minHeap->array =
      (struct Node **)malloc(minHeap->capacity * sizeof(struct Node *));
  return minHeap;
}

// Swap function to heapify
void swap(struct Node **a, struct Node **b) {
  struct Node *t = *a;
  *a = *b;
  *b = t;
}

// Minimum Heapify
void minHeapify(struct MinHeap *minHeap, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < minHeap->size &&
      minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size &&
      minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swap(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

// Check if size of minheap is 1
int checkSizeOne(struct MinHeap *minHeap) { return (minHeap->size == 1); }

// Getting the min frequency node from minheap
struct Node *getMin(struct MinHeap *minHeap) {
  struct Node *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[minHeap->size - 1];

  --minHeap->size;
  minHeapify(minHeap, 0);

  return temp;
}

// Inserting of new node made from 2 minimum frequency nodes in heap
void insertHeapNode(struct MinHeap *minHeap, struct Node *minHeapNode) {
  ++minHeap->size;
  int i = minHeap->size - 1;

  while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }

  minHeap->array[i] = minHeapNode;
}

// Building min heap
void buildHeap(struct MinHeap *minHeap) {
  int n = minHeap->size - 1;
  int i;

  for (i = (n - 1) / 2; i >= 0; --i)
    minHeapify(minHeap, i);
}

// Checking if the node is a leaf node
int isLeaf(struct Node *root) { return !(root->left) && !(root->right); }

struct MinHeap *constructHeap(map<char, int> freq, int size) {
  map<char, int>::iterator itr;
  struct MinHeap *minHeap = createMinHeap(size);
  int i = 0;

  for (itr = freq.begin(); itr != freq.end(); ++itr) {
    minHeap->array[i] = newNode(itr->first, itr->second);
    i++;
  }
  minHeap->size = size;
  buildHeap(minHeap);

  return minHeap;
}

// Building Huffman Tree
struct Node *buildHfTree(map<char, int> freq, int size) {
  struct Node *left, *right, *top;
  struct MinHeap *minHeap = constructHeap(freq, size);

  while (!checkSizeOne(minHeap)) {
    left = getMin(minHeap);
    right = getMin(minHeap);

    top = newNode('#', left->freq + right->freq);

    top->left = left;
    top->right = right;

    insertHeapNode(minHeap, top);
  }
  return getMin(minHeap);
}

// Mapping the codes
void mapCodes(int arr[], int n, char ch) {

  int i;
  string bit;
  for (i = 0; i < n; ++i) {
    bit += to_string(arr[i]);
  }

  cout << bit;

  code.insert({ch, bit});
  cout << "\n";
}

// Get codes for each character
void getCodes(struct Node *root, int arr[], int top) {
  if (root->left) {
    arr[top] = 0;
    getCodes(root->left, arr, top + 1);
  }

  if (root->right) {
    arr[top] = 1;
    getCodes(root->right, arr, top + 1);
  }
  if (isLeaf(root)) {
    cout << "\t" << root->item << "  | ";
    mapCodes(arr, top, root->item);
  }
}

// Encoding the given file
Node *encode(map<char, int> freq, int size) {

  struct Node *root = buildHfTree(freq, size);
  int arr[MAX_TREE_HT], top = 0;
  getCodes(root, arr, top);

  return root;
}

// Reading the file
map<char, int> readFile(string filename) {
  ifstream file;
  map<char, int> freq;

  string line;
  file.open(filename);

  if (!file) {
    cout << "No such file";
  } else {
    while (getline(file, line, '\0')) {
      for (char ch : line) {
        if (freq.find(ch) != freq.end())
          freq[ch]++;
        else
          freq.insert({ch, 1});
      }
    }
  }
  file.close();
  return freq;
}

// Converting into bits
void writeBit(ofstream &file2, int bit) {
  if (bit)
    bit_buffer |= (1 << current_bit);
  current_bit++;
  if (current_bit == 8) {
    bit_string += bit_buffer;
    current_bit = 0;
    bit_buffer = 0;
  }
}

// Deleting the buffer space
void flushBits(ofstream &file) {
  while (current_bit) {
    writeBit(file, 0);
    padding++;
  }
}

// Writing the compressed file
string writeFile(string filename, int should_add_header) {
  ifstream file1;
  string line;
  file1.open(filename);
  string out;

  ofstream file2;
  file2.open("compressed.txt");

  while (getline(file1, line)) {
    line += '\n';
    for (char ch : line) {
      bitset<sizeof(unsigned long) * 8> bits(code[ch]);
      string bin_code = code[ch];

      int bit;
      for (int i = 0; i < bin_code.length(); i++) {
        bit = bin_code[i] == '1' ? 1 : 0;
        writeBit(file2, bit);
        out += bin_code[i];
      }
    }
  }
  flushBits(file2);
  if (should_add_header) {
    int size = code.size();
    file2.write((char *)&padding, sizeof(int));
    file2.write((char *)&size, sizeof(int));
    for (map<char, string>::iterator i = code.begin(); i != code.end(); i++) {
      file2.write((char *)&i->first, 1);
      int len = i->second.size();
      file2.write((char *)&len, sizeof(int));
      file2.write((char *)i->second.c_str(), i->second.size());
    }
  }
  file2.write((char *)bit_string.c_str(), bit_string.size());
  return out;
}

// Convert binary data into bit-string
string convert_to_string(string name, int should_get_header, int &padding,
                         map<char, string> &codes) {
  ifstream fin(name);
  char c;
  string res;

  if (should_get_header) {
    int size;
    fin.read((char *)&padding, sizeof(int));
    fin.read((char *)&size, sizeof(int));
    for (int i = 0; i < size; i++) {
      int sz;
      char a;
      string s;
      fin.read((char *)&a, sizeof(char));
      fin.read((char *)&sz, sizeof(int));
      char *value = (char *)malloc(sz + 1);
      fin.read(value, sz);
      value[sz] = '\0';
      codes[a] = value;
    }
  }
  while (fin.get(c)) {
    for (int i = 0; i < 8; i++)
      res += char((((c >> i) & 1)) + 48);
  }
  return res;
}

// Decoding function to show maximum potential of the algorithm
void decodeMax(string in, ofstream &fout, Node *root, int &index) {
  if (root == NULL)
    return;

  if (root->left == NULL && root->right == NULL) {
    fout << root->item;
    return;
  }

  index++;
  if (in[index] == '0')
    decodeMax(in, fout, root->left, index);
  else
    decodeMax(in, fout, root->right, index);
}

// Decoding function to show real world scenario
void decodeRealWorld(string in, ofstream &fout, map<char, string> codes,
                     int padding) {
  string bit = "";
  map<string, char> reversecodes;
  string buffer;

  for (map<char, string>::iterator i = codes.begin(); i != codes.end(); i++) {
    reversecodes[i->second] = i->first;
  }

  for (int i = 0; i < in.size() - padding; i++) {
    bit += string(1, in[i]);
    if (reversecodes.find(bit) != reversecodes.end()) {
      buffer += (reversecodes[bit]);
      bit = "";
    }
  }

  fout << buffer;
}

// Driver Function
int main() {
  string name;
  map<char, int> freq;
  int choice;

  cout << "Choose an operation: " << endl;
  cout << "1. Compress a file" << endl;
  cout << "2. Decompress a file" << endl;
  cout << "3. Compress and Decompress a file" << endl;
  cin >> choice;

  switch (choice) {
  case 1: {
    cout << "Enter the file name: ";
    cin >> name;

    freq = readFile(name);
    int size = freq.size();

    encode(freq, size);
    string out = writeFile(name, 1);
    break;
  }
  case 2: {
    map<char, string> m;
    int padding = 0;
    cout << "Enter the file name: ";
    cin >> name;
    string in = convert_to_string(name, 1, padding, m);

    ofstream fout("decompressed.txt");
    decodeRealWorld(in, fout, m, padding);
    break;
  }
  case 3: {
    cout << "Enter the file name: ";
    cin >> name;

    freq = readFile(name);
    int size = freq.size();

    Node *root = encode(freq, size);
    string out = writeFile(name, 0);

    map<char, string> m;
    int padding_placeholder = 0;
    string in = convert_to_string("compressed.txt", 0, padding_placeholder, m);
    ofstream fout("decompressed.txt");
    for (int i = -1; i < (int)in.size() - padding - 2;) {
      decodeMax(in, fout, root, i);
    }
    break;
  }
  }
}
