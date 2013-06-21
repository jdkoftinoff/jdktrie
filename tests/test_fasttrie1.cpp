#include <iostream>
#include <string>
#include <memory.h>

#include "jdktrie/fasttrie.h"


typedef jdk_fasttree<int,128,char> my_tree;

template <class TREE>
struct my_iterator : public TREE::iterator_callback
{
  bool operator () ( typename TREE::key_t *key, int len, typename TREE::value_t value )
  {
    if( value!=0 )
    {      
      for( int i=0; i<len; ++i )
      {
        std::cout << char(key[i]);
      }
      std::cout << std::endl;
    }
    return true;
  }
};

my_tree::value_t find( my_tree &tree, const char *str_to_find )
{
  int len_of_str=strlen(str_to_find);
  return tree.find( str_to_find, len_of_str );
}


int main(int argc, char **argv )
{
  my_tree tree;
  
  tree.add( "testing1", strlen("testing1"), 1 );
  tree.add( "testtree", strlen("testtree"), 2 );
  tree.add( "jeffrey", strlen("jeffrey"), 3 );
  tree.add( "macintosh", strlen("macintosh"), 4 );
  
  tree.remove( "jeffrey", strlen("jeffrey") );
 
  std::cout << "Dumping tree:" << std::endl;

  my_iterator<my_tree> i;
  my_tree::key_t buf[1024];
  tree.iterate( buf, 1024, i );

  std::cout << "Finding 'macintosh' in tree:" << std::endl;  
  std::cout << "Value is: " << find(tree,"macintosh") << std::endl;
  
  return 0;
}
