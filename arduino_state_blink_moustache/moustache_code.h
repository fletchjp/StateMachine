// moustache_code.h
// Extra codes for moustache.

#ifndef MOUSTACHE_CODE_H
#define MOUSTACHE_CODE_H

// This returns the size of a moustache_variable_t array and also of an array of arrays.
// It can be used to control output from rendering the array of arrays
template <typename T, size_t n>
size_t moustache_size(T (&values)[n])
{
  return n;
}

template <typename T, typename V, size_t n>
void moustache_value(T (&values)[n], size_t i, const V &v)
{
  if (i < n) values[i].value = String(v);
  //return n;
}

// This will not compile in the *.ino file with format as the first argument - why is not clear to me.
// It works in a header file.
// This will change one value and render the file in one go.
template <typename T, typename V, size_t n>
String moustache_render_value(const String &format, T (&values)[n], size_t i, const V &v )
{
    if (i < n) values[i].value = String(v);
    //moustache_value(values,i,v);
    String s =  moustache_render(format,values);
    return s;
}


#endif

