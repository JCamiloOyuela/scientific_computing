#include "Zipf.hh"


void
add_word( std::string const & w, std::map<std::string,int> & m ) {
  // check if w is into m
  auto res = m.find(w);
  if ( res != m.end() ) {
    ++res->second; // increment
  } else {
    m[w] = 1;
  }
}

bool
is_letter_or_italian_accent( char c ) {
  return ( c >= 'a' && c <= 'z' ) ||
         ( c >= 'A' && c <= 'Z' );
}

  // return s == "à" || s == "è" || s == "é" ||
  //        s == "ì" || s == "ò" || s == "ù" ||
  //        s == "À" || s == "È" || s == "É" ||
  //        s == "Ì" || s == "Ò" || s == "Ù";

// Convert an uppercase letter to lowercase.
char to_lowercase(char c)
{
    if (c >= 'A' && c <= 'Z')
    {
        return c - (65 - 97);
    }
    return c;
}

/* optimized */
/*
void
add_word( std::string const & w, std::map<std::string,int> & m ) {
  ++m[w];
}
*/

/*
  command arg1 arg2 ...
  
  argc = number of argument+1
  argv[] = { "command", "art1", ... }
*/
int
main( int argc, char *argv[] ) {
    // map:
    // word -> frequency
    std::map<std::string,int> words;

    // ------------------------------------------------------
    // check input arguments
    // ------------------------------------------------------

    if ( argc != 2 ) {
      std::cerr << "Usage: " << argv[0] << " file!\n";
      return 1;
    }
    
    // get file name
    std::string file_name = argv[1]; // copy file name to string file_name
    // open file
    std::ifstream file( file_name );
    if ( !file ) {
      std::cerr << "Cannot open file: " << file_name << "\n";
      return 1;
    }
    // variable for each line
    std::string line;

    // ------------------------------------------------------
    // read file line by line
    // ------------------------------------------------------
    while ( std::getline( file, line ) ) {
      // here line contains one full line from the file
      // std::cout << line << '\n';
      // update words
      std::string w;
      for ( auto c : line ) {
        if ( is_letter_or_italian_accent( c ) ) {
          w += to_lowercase(c);
        } else {
          if ( w.length() > 1 ) add_word( w, words );
          w = "";
          // std::cout << w << "\n";
        }
      }
      if ( w.length() > 1 ) add_word( w, words );
    }
    
    file.close();

  // ------------------------------------------------------
  // copy map into vector
  // ------------------------------------------------------

  // rank is a vector of pairs:
  // first  = word
  // second = frequency
  std::vector<std::pair<std::string,int>> rank;

  for ( auto item : words ) {
    rank.push_back( item );
  }


  // ------------------------------------------------------
  // sort vector by frequency
  // ------------------------------------------------------

  std::sort(
    rank.begin(),
    rank.end(),
    []( std::pair<std::string,int> const & a,
        std::pair<std::string,int> const & b ) {
      return a.second > b.second;
    }
  );

  std::ofstream out("top10.txt");

  for ( int i = 0; i < rank.size(); ++i ) {
    out << i + 1 << " "
        << rank[i].second << " "
        << rank[i].first << "\n";
  }

  out.close();
 
  std::cout << "Number of different words = " << rank.size() << "\n";

  // ------------------------------------------------------
  // print first 10 words
  // ------------------------------------------------------

  int nprint = 10;

  if ( rank.size() < 10 ) {
    nprint = rank.size();
  }

  for ( int i = 0; i < nprint; ++i ) {
    std::cout << rank[i].first << ": " << rank[i].second << "\n";
  }

  std::cout << "All done folks!\n";
  return 0;
}
