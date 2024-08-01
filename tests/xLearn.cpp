
/**
 */

#include <sstream>
#include <utility>
#include <string>
#include <vector>
#include <array>
#include<unordered_set>
#include<algorithm>

using namespace std;

struct test {
};


bool findSimilar(const std::vector<int>& x) {
  unordered_set<int> set;
  int num = x.size();
  printf("num %d\n", num);
  for (int i : x)
  {
    if (set.count(i)){
      printf("true\n");
      return true;
    }
    set.insert(i);

  }
  printf("false\n");
  return false;
}


string reverseVowels(const string& s){
  string result = s;
  vector<int> vowel_positions;
  vector<char> vowels;
  unordered_set<char> vowels_lower = {'a', 'e', 'i', 'o', 'u'};
  unordered_set<char> vowels_upper = {'A', 'E', 'I', 'O', 'U'};

  for (int i =0; i < s.length(); ++i){
    if (vowels_lower.count(s[i]))
    {
      vowel_positions.push_back(i);
      vowels.push_back(s[i]);
    }
    if (vowels_upper.count(s[i]))
    {
      vowel_positions.push_back(i);
      vowels.push_back(s[i]);
    }
  }

  reverse(vowel_positions.begin(), vowel_positions.end());
  for (int i =0; i<vowels.size(); ++i){
    result[vowel_positions[i]] = vowels[i];
  }
  return result;
}

int main(){

//  std::array<float, 100> f;
  int f[100];
  int r[100];

  std::vector<int> x = {1,2,3,1,7,9,0};


  int it =0;
  for (auto& i: f){
    i = it;
    it ++;
  }

  for (int i =0; i<100; ++i){
    r[i] = (i>0) ? r[i-1] + f[i] : f[i];
  }

  findSimilar(x);

  auto s = reverseVowels ("hEllo");

  printf("%s\n", s.c_str());

  return 1;
}
