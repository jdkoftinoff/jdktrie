/*
 *  trie_map.h
 *  trie_map
 *
 *  Created by Jeff Koftinoff on 2006/03/02.
 *  Copyright 2006 J.D. Koftinoff Software, Ltd. All rights reserved.
 *
 */

#ifndef TRIE_MAP_H_
#define TRIE_MAP_H_

#include <iterator>
#include <memory>
#include <stdint.h>

template <bool test_condition>
struct trie_map_compile_time_test
{
  trie_map_compile_time_test()
  {
    switch(false)
    { 
    case false:
    case test_condition:
    break;
    }
  }
};

template <unsigned int VALUE>
struct trie_map_test_for_power_of_two 
:  private trie_map_compile_time_test< ((VALUE & (VALUE-1))==0) >
{
};

template <
  typename KEY_T, 
  typename MAPPED_T, 
  size_t KEY_ELEMENT_RANGE=256,
  typename ALLOCATOR_T = std::allocator< std::pair< const KEY_T, MAPPED_T> >
  > 
class trie_map : 
  private trie_map_test_for_power_of_two<KEY_ELEMENT_RANGE>
{
public:
  
  typedef KEY_T key_type;    
  typedef std::pair< const KEY_T, MAPPED_T>  value_type;
  typedef MAPPED_T mapped_type;
  typedef ALLOCATOR_T allocator_type;
  typedef typename ALLOCATOR_T::reference reference;
  typedef typename ALLOCATOR_T::const_reference const_reference;
  
  typedef int size_type;  /* TODO: check */
  typedef int difference_type; /* TODO: check */
  
  typedef typename ALLOCATOR_T::pointer pointer;
  typedef typename ALLOCATOR_T::const_pointer const_pointer;
  
  //typedef std::reverse_iterator<iterator> reverse_iterator;
  //typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
  
  enum 
  {
    key_element_range_mask = KEY_ELEMENT_RANGE-1,
    key_element_range = KEY_ELEMENT_RANGE
  };
  
  typedef typename key_type::value_type key_element_type;
  typedef trie_map<key_type,mapped_type,key_element_range,allocator_type> trie_map_type;


  struct iterator
  {
    inline explicit iterator() 
      :
      top_trie(0),
      parent(0),
      item(0)
    {
    }
    
    inline iterator( 
      trie_map_type *top_trie_,
      trie_map_type *parent_,
      key_element_type item_
      )
      :
      top_trie( top_trie_ ),
      parent( parent_ ),
      item( item_ & key_element_range_mask )
    {
    }
    
    
    inline iterator( const iterator &other )
      : 
      top_trie( other.top_trie ),
      parent( other.parent ),
      item( other.item )
    {
    }
    
    inline iterator & operator = ( const iterator &other )
    {
      top_trie = other.top_trie;
      parent = other.parent;
      item = other.item;
      return *this;
    }
    
    inline bool operator == ( const iterator &other )
    {
      return top_trie == other.top_trie && item == other.item && parent == other.parent;
    }
    
    inline bool operator != ( const iterator &other )
    {
      return ! operator == ( other );
    }
    
    inline iterator operator ++ (int)
    {
      const_iterator current(*this);
      next();
      return current;
    }

    inline iterator operator ++ ()
    {
      next();
      return *this;
    }
    
    inline value_type operator * () const
    {
      return *(parent->table[int(item)]->value);
    }
     
    inline value_type * operator ->() const
    {
      return (parent->table[int(item)]->value);
    }
          
    private:
    
    inline void next()
    {
      if( item!=key_element_range_mask )
      {
        *this = parent->begin( top_trie, item+1 );
      }
      else
      {
        if( parent )
        {
          item = parent->key_element;
          parent=parent->parent;
          next();
        }
        else
        {
          *this = top_trie->end();
        }
      }
    }

    trie_map_type *top_trie;
    trie_map_type *parent;
    key_element_type item;
  };

  
  struct const_iterator
  {
    inline explicit const_iterator() 
      :
      top_trie(0),
      parent(0),
      item(0)
    {
    }
    
    inline const_iterator( 
      const trie_map_type *top_trie_,
      const trie_map_type *parent_,
      key_element_type item_
      )
      :
      top_trie( top_trie_ ),
      parent( parent_ ),
      item( item_ & key_element_range_mask )
    {
    }
    
    
    inline const_iterator( const const_iterator &other )
      : 
      top_trie( other.top_trie ),
      parent( other.parent ),
      item( other.item )
    {
    }
    
    inline const_iterator & operator = ( const const_iterator &other )
    {
      top_trie = other.top_trie;
      parent = other.parent;
      item = other.item;
      return *this;
    }
    
    inline bool operator == ( const const_iterator &other )
    {
      return top_trie == other.top_trie && item == other.item && parent == other.parent;
    }
    
    inline bool operator != ( const const_iterator &other )
    {
      return ! operator == ( other );
    }
    
    inline const_iterator operator ++ (int)
    {
      const_iterator current(*this);
      next();
      return current;
    }

    inline const_iterator operator ++ ()
    {
      next();
      return *this;
    }
    
    inline const value_type operator * () const
    {
      return *(parent->table[int(item)]->value);
    }
     
    inline const value_type * operator ->() const
    {
      return (parent->table[int(item)]->value);
    }
          
    private:
 
    inline void next()
    {
      if( item!=key_element_range_mask )
      {
        *this = parent->begin( top_trie, item+1 );
      }
      else
      {
        if( parent )
        {
          item = parent->key_element;
          parent=parent->parent;
          next();
        }
        else
        {
          *this = top_trie->end();
        }
      }
    }
        
    const trie_map_type *top_trie;
    const trie_map_type *parent;
    key_element_type item;
  };
  
  
  inline bool operator == ( const trie_map_type &other ) const
  {
    bool is_equal=true;
    
    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if( table[i]!=0 && other.table[i]!=0 )
      {
        is_equal=table[i]->operator == ( other.table[i] );
        if( !is_equal )
          break;
      }
      else if( table[i]!=0 || other.table[i]!=0 )
      {
        is_equal = false;
        break;
      }
    }
    return is_equal;
  }
  
  inline bool operator != ( const trie_map_type &other ) const
  {
    return ! operator == ( other );
  }
  
  inline bool empty() const
  {
    bool is_empty=true;
    
    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if( table[i] )
      {
        is_empty=false;
        break;
      }
    }
    return is_empty;
  }
  
  inline size_t size() const
  {
    size_t total_size=0;
    
    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if (table[i]) 
      {
        total_size+=table[i]->size();
      }
    }
    return total_size;
  }
  
  inline void clear()
  {
    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if( table[i] )
      {
        delete table[i];
        table[i] = 0;
      }
    }    
  }
  
  inline trie_map_type & operator = ( const trie_map_type &other )
  {
    clear();

    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if( other.table[i] )
        table[i] = new trie_map_type( *other.table[i], allocator );
    }

    return *this;
  }

  inline trie_map( const allocator_type &allocator_ = allocator_type() )
    : 
    allocator( allocator_ ),
    value(0),
    parent(0),
    key_element( 0 )
  {
      for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
      {
        table[i] = 0;  
      }    
  }
  
  inline trie_map( trie_map_type *parent_, key_element_type key_element_, const allocator_type &allocator_ = allocator_type() )
    : 
    allocator( allocator_ ),
    value(0),
    parent(parent_),
    key_element( key_element_ )
  {
      for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
      {
        table[i] = 0;  
      }    
  }
  
  inline trie_map( const trie_map_type &other, trie_map_type *parent_=0, const allocator_type &allocator_ = allocator_type()  )
    : 
    allocator(allocator_),
    value(other.value),
    parent(parent_)
  {
      for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
      {
        if( other.table[i] )
        {
          table[i] = new trie_map_type( *other.table[i], allocator_, this );
        }
        else
        {
          table[i] = 0;  
        }    
      }
  }
  
  
  inline ~trie_map()
  {
    for( size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
    {
      if( table[i] )
        delete table[i];
    }
    delete value;
  }
  
  inline std::pair< iterator, bool> insert(const value_type &v )
  {
    bool r=_add( v.first, v.second );
    return std::make_pair( find( v.first ), r );
  } 
  
  inline bool _add( const key_type &k, const mapped_type &v )
  {
    trie_map_type * t = this;
    typename key_type::const_iterator pos(k.begin());
    typename key_type::const_iterator end(k.end());
    
    for( ; pos!=end; ++pos )
    {
      size_t c = static_cast<size_t>(*pos) & key_element_range_mask;
      
      if( !t->table[ c ] )
      {
        t->table[c] = new trie_map_type(t,*pos,allocator);
      }
      t = t->table[c];
    }
    
    bool r=false;
    if( !t->value )
    {
      t->value = new value_type(k,v);
      r=true;
    }
    return r;
  }
  
  inline mapped_type & operator[](const key_type& k)
  {
    iterator i = find( k );
    
    if( i==end() )
    {
      _add( k, mapped_type() );
      i = find(k);
    }
    return i->second;
  }


  inline iterator find( const key_type &k )
  {
    trie_map_type * t = this;
    typename key_type::const_iterator pos(k.begin());
    typename key_type::const_iterator end(k.end());
    
    for( ; pos!=end; ++pos )
    {
      size_t c = static_cast<size_t>(*pos) & key_element_range_mask;
      t = t->table[ c ];
      if( !t )
      {
        return iterator(this,0,0);
      }    
    }
    
    return iterator( this, t->parent, t->key_element);    
  }

  
  inline const_iterator find( const key_type &k ) const
  {
    const trie_map_type * t = this;
    typename key_type::const_iterator pos(k.begin());
    typename key_type::const_iterator end(k.end());
    
    for( ; pos!=end; ++pos )
    {
      size_t c = static_cast<size_t>(*pos) & key_element_range_mask;
      t = t->table[ c ];
      if( !t )
      {
        return const_iterator(this,0,0);
      }    
    }
    
    return const_iterator( this, t->parent, t->key_element);    
  }
    
  inline bool erase( const const_iterator &i )
  { 
    // TODO
    return false;
  }

#if 0
  inline const_iterator find( const key_type &k )
  {
    return const_end();
  }
#endif

  inline bool erase( const key_type &k )
  {
    trie_map_type * last_fork = this;
    typename key_type::const_iterator buf(k.begin());
    typename key_type::const_iterator end(k.end());

    size_t last_fork_pos=0;
    size_t pos=0;
    trie_map_type * t = this;
    
    
    // follow the branch, but remember where the last fork in the branch is
    
    for( ; buf!=end; ++buf )
    {
      size_t entry_count=0;
      for(size_t i=0; i<KEY_ELEMENT_RANGE; ++i )
      {
        entry_count += static_cast<size_t>( t->table[i]!=0 );
      }
      
      if( entry_count>1 )
      {
        last_fork_pos = pos;
        last_fork = t;
      }
      
      size_t c = static_cast<size_t>(*buf) & key_element_range_mask;
      t = t->table[ c ];
      if( !t )
      {
        return false;
      }
    }    
    
    // ok, we found a match. Now delete the branch starting from our last fork position
    
    size_t c = static_cast<size_t>( k[last_fork_pos] ) & key_element_range_mask;
    trie_map_type **kill_point = &last_fork->table[ c ];
    delete *kill_point;
    *kill_point = 0;
    
    return true;
  }
  
  inline iterator begin( trie_map_type *top_level=0, size_t self_item=0 ) 
  {
    // find first item
    if( top_level==0 )
      top_level = this;
        
    for( size_t i=self_item; i<key_element_range; ++i )
    {
      if( table[ i ] )
      {
        return table[i]->begin( top_level, 0 );
      }
    }
    if( value )
    {
      return iterator( top_level, parent, key_element );
    }
    else if( parent ) // watch it here, edge condition when at end of array. this is a bug!
    {
      return parent->begin( top_level, (key_element+1) & key_element_range_mask );
    }
    else
      return iterator(top_level,0,0);
  }
  
  inline iterator end()
  {
    return iterator(this,0,0);
  }

  inline const_iterator begin( const trie_map_type *top_level=0, size_t self_item=0 ) const
  {
    // find first item
    if( top_level==0 )
      top_level = this;
          
    for( size_t i=self_item; i<key_element_range; ++i )
    {
      if( table[ i ] )
      {
        return table[i]->begin( top_level, 0 );
      }
    }
    if( value )
    {
      return const_iterator( top_level, parent, key_element );
    }
    else if( parent ) // watch it here, edge condition when at end of array. this is a bug!
    {
      return parent->begin( top_level, (key_element+1) & key_element_range_mask );
    }
    else
      return const_iterator(top_level,0,0);
  }
  
  inline const_iterator end() const
  {
    return const_iterator(this,0,0);
  }
private:
  const allocator_type allocator;
  trie_map_type * table[KEY_ELEMENT_RANGE];
  trie_map_type * parent;
  key_element_type key_element;
  value_type *value;
  
};

#endif
