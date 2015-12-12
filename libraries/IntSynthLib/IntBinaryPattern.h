#ifndef INTBINARYPATTERN
#define INTBINARYPATTERN

namespace isl
{

/*!
 * T must be unsigned!
 */
template<class T>
class CBinaryPattern
{
public:
    CBinaryPattern()
     : m_Pattern(0)
     , m_PatternSize(PatternCapacity)
    {}

    void Reset(T Pattern)
    {
        m_Pattern = Pattern;
    }

    void Next()
    {
        ++m_Pattern;
    }

    void Prev()
    {
        --m_Pattern;
    }

    void JumpNext(T Jump)
    {
        m_Pattern += Jump;
    }

    void JumpPrev(T Jump)
    {
        m_Pattern -= Jump;
    }

    void RotateNext()
    {
        // TODO use size??
        m_Pattern = (m_Pattern<<1) | (m_Pattern >> (PatternCapacity-1));
    }

    void RotatePrev()
    {
        m_Pattern = (m_Pattern>>1) | (m_Pattern << (PatternCapacity-1));
    }

    void Invert()
    {
        m_Pattern = ~m_Pattern;
    }

    void Mirror()
    {
        // http://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
        T v = m_Pattern;
        T r = v;

        int s = PatternCapacity - 1; // extra shift needed at end

        for (v >>= 1; v; v >>= 1)
        {
          r <<= 1;
          r |= v & 1;
          s--;
        }
        r <<= s; // shift when v's highest bits are zero

        m_Pattern = r;
    }

    T Get() const
    {
        return m_Pattern;
    }

    bool GetBit(int Bit)
    {
        return m_Pattern & 1<<Bit;
    }

private:
    const int PatternCapacity = sizeof(T)*8;

    T m_Pattern;
    T m_PatternSize;
};

}

#endif // INTBINARYPATTERN
