//EMBR is a real time character animation engine.
//Copyright (C) 2010 Alexis Heloir
//
//This program is free software: you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation, version 3
//of the License.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY, without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this program.  If not, see
//<http://www.gnu.org/licenses/>.


#ifndef C_POINTER_H
#define C_POINTER_H

/*  class for counted pointer
 *  counts number of owners
 *  deletes object when count == 0 
 */
template <class T>
class CPointer {
  private:
    T* ptr;        
    long* count;   // owners

  public:
    // initialize pointer with existing pointer
    
    explicit CPointer (T* p=0)
     : ptr(p), count(new long(1)) {
    }

    // copy pointer and add owner)
    CPointer (const CPointer<T>& p) throw()
     : ptr(p.ptr), count(p.count) {
        ++*count;
    }

    // destructor (delete value if this was the last)
    ~CPointer () throw() {
        dispose();
    }

    
    CPointer<T>& operator= (const CPointer<T>& p) throw() {
        if (this != &p) {
            dispose();
            ptr = p.ptr;
            count = p.count;
            ++*count;
        }
        return *this;
    }

    
    T& operator*() const throw() {
        return *ptr;
    }
    T* operator->() const throw() {
        return ptr;
    }

  private:
    void dispose() {
        if (--*count == 0) {
             delete count;
             delete ptr;
        }
    }
    
};

#endif /* C_POINTER_H */

