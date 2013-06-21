#ifndef JDKTRIE_FASTTREE_H
#define JDKTRIE_FASTTREE_H


template <typename VALUE, int RANGE, typename KEYTYPE>
struct jdk_fasttree
{
  typedef VALUE value_t;
  enum 
  {
    range_mask = RANGE-1,
    range = RANGE
  };
  typedef KEYTYPE key_t;  
  typedef jdk_fasttree<VALUE,RANGE,KEYTYPE> type_t;
     
  value_t result;
  type_t * parent;
  type_t * table[RANGE];

  struct iterator_callback
  {
    virtual ~iterator_callback() {}
    virtual bool operator() ( key_t *key, int len, value_t value ) = 0;
  };
  
    
  inline jdk_fasttree()
    : result(),
    parent(0)
  {
    for( int i=0; i<range; ++i )
    {
      table[i] = 0;  
    }    
  }
  
  inline jdk_fasttree( type_t *parent_ )
    : result(), 
    parent( parent_ )
  {
    for( int i=0; i<range; ++i )
    {
      table[i] = 0;  
    }    
  }
  
  
  inline ~jdk_fasttree()
  {
    for( int i=0; i<range; ++i )
    {
      if( table[i] )
        delete table[i];
    }
  }
    
  
  inline void add( const key_t *buf, int len, value_t v )
  {
    type_t * t = this;
    
    while( len-- )
    {
      key_t c = (*buf)&range_mask;
      
      if( !t->table[int(c)] )
      {
        t->table[int(c)] = new type_t(t);
      }
      t = t->table[int(c)];
      ++buf;
    }
    t->result = v;
  }
  
  inline value_t find( const key_t *buf, int len )
  {
    type_t * t = this;
    
    while( len-- )
    {
      t = t->table[ *(buf++)&(range_mask) ];
      if( !t )
      {
        return 0;
      }    
    }

    return t->result;    
  }

  inline const value_t find( const key_t *buf, int len ) const
  {
    type_t * t = this;
    
    while( len-- )
    {
      t = t->table[ *(buf++)&(range_mask) ];
      if( !t )
      {
        return 0;
      }    
    }

    return t->result;    
  }
  
  inline bool remove( const key_t *buf, int len )
  {
    type_t * last_fork = this;
    int last_fork_pos=0;
    int pos=0;
    type_t * t = this;
    

    // follow the branch, but remember where the last fork in the branch is

    while( len-- )
    {
      int entry_count=0;
      for(int i=0; i<range; ++i )
      {
        entry_count += (int)(t->table[i]!=0);
      }
    
      if( entry_count>1 )
      {
        last_fork_pos = pos;
        last_fork = t;
      }
      
      t = t->table[ buf[pos]&(range_mask) ];
      if( !t )
      {
        return false;
      }
      
         ++pos;  
    }    

    // ok, we found a match. Now delete the branch starting from our last fork position
    
    type_t **kill_point = &last_fork->table[ buf[last_fork_pos]&(range_mask) ];
    delete *kill_point;
    *kill_point = 0;
    
    return true;
  }

  inline bool iterate( key_t *buf, int max_len, iterator_callback &cb, int buf_pos=0 )
  {
    if( buf_pos>=max_len )
    {
      return false;
    }
    
    if( buf_pos>0 )
    {      
      if( !cb( buf, buf_pos, result ) )
      {
        return false;
      }
    }        
    
    for(int i=0; i<range; ++i )
    {
      if( table[i] )
      {        
          buf[buf_pos]=i;
        if( !table[i]->iterate( buf, max_len, cb, buf_pos+1 ) )
        {
          return false;
        }        
      }
    }
    return true;
  }  
};



#endif

