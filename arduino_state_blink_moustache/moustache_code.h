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

// Note: all value changes are checked for the size of values.
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

// This will change two values and render the file in one go.
template <typename T, typename V, typename W, size_t n>
String moustache_render_value2(const String &format, T (&values)[n], size_t i, const V &v, size_t j, const W &w )
{
    if (i < n) values[i].value = String(v);
    if (i < n) values[j].value = String(w);
    //moustache_value(values,i,v);
    String s =  moustache_render(format,values);
    return s;
}


#endif

