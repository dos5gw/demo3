#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
 
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
 
// 无锁缓冲队列.
class circular_buffer
{
public:
    circular_buffer ( int buf_size )
            : m_buffer_size ( buf_size )
            , m_circle_buffer ( NULL )
            , m_write_p ( 0 )
            , m_read_p ( 0 )
    {
        m_circle_buffer = new char[m_buffer_size];
    }
 
    ~circular_buffer()
    {
        if ( m_circle_buffer )
            delete[] m_circle_buffer;
        m_circle_buffer = NULL;
    }
 
    void clear()
    {
        m_write_p = 0;
        m_read_p = 0;
    }
 
    unsigned int size()
    {
        return m_buffer_size - ( m_write_p - m_read_p );
    }
 
    unsigned int used()
    {
        return m_write_p - m_read_p;
    }
 
    unsigned int put_data ( char* buffer, unsigned int len )
    {
        unsigned int l;
        len = _min ( len, m_buffer_size - m_write_p + m_read_p );
        /* first put the data starting from fifo->in to buffer end */
        l = _min ( len, m_buffer_size - ( m_write_p & ( m_buffer_size - 1 ) ) );
        memcpy ( m_circle_buffer + ( m_write_p & ( m_buffer_size - 1 ) ), buffer, l );
        /* then put the rest (if any) at the beginning of the buffer */
        memcpy ( m_circle_buffer, buffer + l, len - l );
        m_write_p += len;
        return len;
    }
 
    unsigned int get_data ( char* buffer, unsigned int len )
    {
        unsigned int l;
        len = _min ( len, m_write_p - m_read_p );
        /* first get the data from fifo->out until the end of the buffer */
        l = _min ( len, m_buffer_size - ( m_read_p & ( m_buffer_size - 1 ) ) );
        memcpy ( buffer, m_circle_buffer + ( m_read_p & ( m_buffer_size - 1 ) ), l );
        /* then get the rest (if any) from the beginning of the buffer */
        memcpy ( buffer + l, m_circle_buffer, len - l );
        m_read_p += len;
        return len;
    }
 
protected:
    inline unsigned int _max ( unsigned int a, unsigned int b )
    {
        return max ( a, b );
    }
 
    inline unsigned int _min ( unsigned int a, unsigned int b )
    {
        return min ( a, b );
    }
 
private:
    int m_buffer_size;
    char* m_circle_buffer;
    unsigned int m_write_p;
    unsigned int m_read_p;
};

