#pragma once

#include<stdexcept>
#include<iterator>
namespace gl {
    template<typename CBType>
    class _circular_buffer_const_iterator
    {
    public:
        typedef _circular_buffer_const_iterator<CBType> this_type;
        typedef typename CBType::value_type             value_type;
        typedef typename CBType::difference_type        difference_type;
        typedef typename CBType::const_pointer			pointer;
        typedef typename CBType::const_reference		reference;
        typedef std::random_access_iterator_tag         iterator_category;
    public:
        _circular_buffer_const_iterator(const CBType* cb) :d_cb(cb),d_ptr(nullptr) {
            
        }
        _circular_buffer_const_iterator(const CBType* cb , typename CBType::pointer ptr) : d_cb(cb),d_ptr(ptr) {
            
        }
        reference operator*() const {
            return *d_ptr;
        }
        pointer operator->() const {
            return &(**this);
        }
        this_type& operator++() {
            d_cb->increment(d_ptr);
            return *this;
        }
        this_type operator++(int) {
            this_type temp = *this;
            ++(*this);
            return temp;
        }
        this_type& operator+=(difference_type n){
            d_ptr = d_cb->add(d_ptr,n);
            return *this;
        }
        this_type operator+(difference_type n) const{
            this_type temp = *this;
            temp += n;
            return temp;
        }
        this_type& operator--() {
            d_cb->decrement(d_ptr);
            return *this;
        }
        this_type operator--(int) {
            this_type temp = *this;
            --(*this);
            return temp;
        }
        this_type& operator-=(difference_type n) {
            d_ptr = d_cb->sub(d_ptr,n);
            return *this;
        }
        this_type operator-(difference_type n) const {
            this_type temp = *this;
            temp -= n;
            return temp;
        }
        difference_type operator-(const this_type& right) const {
            return d_cb->distance(d_ptr,right.d_ptr);
        }
        reference operator[](difference_type n) {
            return *(*this + n);
        }
        bool operator==(const this_type& right) {
            return this->d_ptr == right.d_ptr;
        }
        bool operator!=(const this_type& right) {
            return !(*this == right);
        }
        bool operator<(const this_type& right) {
            return d_cb->less(this->d_ptr , right.d_ptr);
        }
        bool operator>(const this_type& right) {
            return right < *this;
        }
        bool operator<=(const this_type& right) {
            return !(right < *this);
        }
        bool operator>=(const this_type& right) {
            return !(*this < right);
        }
    private:
        typename CBType::pointer d_ptr;
        const CBType* d_cb;
    };
    template<typename CBType>
    class _circular_buffer_iterator :public _circular_buffer_const_iterator<CBType>
    {
    public:
        typedef _circular_buffer_iterator<CBType>	this_type;
        typedef _circular_buffer_const_iterator<CBType> base_type;
        typedef typename CBType::value_type			value_type;
        typedef typename CBType::difference_type	difference_type;
        typedef typename CBType::pointer			pointer;
        typedef typename CBType::reference			reference;
        typedef std::random_access_iterator_tag		iterator_category;
    public:
        _circular_buffer_iterator(const CBType* cb) :_circular_buffer_const_iterator<CBType>(cb) {
            
        }
        _circular_buffer_iterator(const CBType* cb,pointer ptr) :_circular_buffer_const_iterator<CBType>(cb,ptr) {
            
        }
        reference operator*() const{
            return (reference)(*get_base_ref());
        }
        pointer operator->() const{
            return &(**this);
        }
        this_type& operator++() {
            ++get_base_ref();
            return *this;
        }
        this_type operator++(int) {
            this_type temp = *this;
            ++(*this);
            return *this;
        }
        this_type& operator+=(difference_type n) {
            get_base_ref() += n;
            return *this;
        }
        this_type operator+(difference_type n) const {
            this_type temp = *this;
            temp += n;
            return temp;
        }
        this_type& operator--() {
            --get_base_ref();
            return *this;
        }
        this_type operator--(int) {
            this_type temp = *this;
            --(*this);
            return temp;
        }
        this_type& operator-=(difference_type n) {
            get_base_ref() -= n;
            return *this;
        }
        this_type operator-(difference_type n) const {
            this_type temp = *this;
            temp -= n;
            return temp;
        }
        difference_type operator-(const base_type& right) const {
            return  get_base_ref() - right;
        }
        reference operator[](difference_type n) {
            return (reference)(get_base_ref()[n]);
        }
        
    private:
        base_type& get_base_ref() {
            return *(base_type*)this;
        }
        const base_type& get_base_ref() const {
            return *(base_type*)this;
        }
    };
    template<typename T, typename Alloc=std::allocator<T>>
    class circular_buffer {
    public:
        typedef Alloc										allocator_type;
        typedef circular_buffer<T, Alloc>					this_type;
        typedef typename allocator_type::value_type			value_type;
        typedef typename allocator_type::size_type			size_type;
        typedef	typename allocator_type::difference_type	difference_type;
        typedef typename allocator_type::pointer			pointer;
        typedef typename allocator_type::const_pointer		const_pointer;
        typedef typename allocator_type::reference			reference;
        typedef typename allocator_type::const_reference	const_reference;
        typedef T&&											rvalue_type;
        typedef _circular_buffer_iterator<this_type>		iterator;
        typedef _circular_buffer_const_iterator<this_type>	const_iterator;
        typedef std::reverse_iterator<iterator>				reverse_iterator;
        typedef std::reverse_iterator<const_iterator>		const_reverse_iterator;
        
        typedef const value_type& param_value_type;
    public:
        explicit circular_buffer(const allocator_type & alloc = allocator_type()) noexcept
            : d_alloc(alloc),d_begin(nullptr),d_end(nullptr),d_first(nullptr),d_last(nullptr){
        }
        explicit circular_buffer(size_type capacity, const allocator_type & alloc= allocator_type())
            : d_alloc(alloc) {
            init(capacity,0);
        }
        circular_buffer(size_type size, param_value_type value, const allocator_type & alloc = allocator_type())
            : d_alloc(alloc) {
            init(size,size);
            std::uninitialized_fill(d_first, d_last , value);
        }
        circular_buffer(size_type capacity, size_type size, param_value_type value,const allocator_type & alloc = allocator_type())
            : d_alloc(alloc) {
            init(capacity,size);
            std::uninitialized_fill(d_first, d_last , value);
        }
        circular_buffer(const circular_buffer< T, Alloc > & other)
            : d_alloc(other.d_alloc) {
            init(other.capacity(),other.size());
            std::uninitialized_copy(other.begin(), other.end(), d_first);
        }
        circular_buffer(circular_buffer< T, Alloc > && other) noexcept
            : d_alloc(other.d_alloc),d_begin(other.d_begin),d_end(other.d_end),d_first(other.d_first),d_last(other.d_last){
            other.d_begin = nullptr;
            other.d_end = nullptr;
            other.d_first = nullptr;
            other.d_last = nullptr;
        }
        template<typename InputIterator>
        circular_buffer(InputIterator first, InputIterator last, const allocator_type & alloc = allocator_type())
        : d_alloc(alloc) {
            size_type size = last - first;
            init(size,size);
            std::uninitialized_copy(first, last, d_first);
        }
        template<typename InputIterator>
        circular_buffer(size_type capacity, InputIterator first, InputIterator last,const allocator_type & alloc = allocator_type())
        : d_alloc(alloc) {
            init(capacity,last-first);
            std::uninitialized_copy(first, last, d_first);
            
        }
        circular_buffer< T, Alloc > & operator=(const circular_buffer< T, Alloc > & other) {
            if(d_begin){
                clear();
                d_alloc.deallocate(d_begin,d_end-d_begin);
            }
            d_alloc = other.d_alloc;
            if(other.capacity() > 0){
                init(other.capacity(),other.size());
                std::uninitialized_copy(other.begin(), other.end(), d_first);
            }else{
                d_begin = nullptr;
                d_end = nullptr;
                d_first = nullptr;
                d_last = nullptr;
            }
        }
        circular_buffer< T, Alloc >& operator=(circular_buffer< T, Alloc > && other) noexcept {
            if(d_begin){
                clear();
                d_alloc.deallocate(d_begin,d_end-d_begin);
            }
            d_alloc = other.d_alloc;
            d_begin = other.d_begin;
            d_end = other.d_end;
            d_first = other.d_first;
            d_last = other.d_last;
            other.d_begin = nullptr;
            other.d_end = nullptr;
            other.d_first = nullptr;
            other.d_last = nullptr;
        }
        ~circular_buffer() noexcept {
            if(d_begin){
                clear();
                d_alloc.deallocate(d_begin,d_end - d_begin);
            }
        }

        allocator_type get_allocator() const {
            return d_alloc;
        }
        allocator_type& get_allocator() {
            return d_alloc;
        }
        
        iterator begin() {
            return iterator(this,d_first);
        }
        iterator end() {
            return iterator(this,d_last);
        }
        const_iterator begin() const {
            return const_iterator(this,d_first);
        }
        const_iterator end() const {
            return const_iterator(this,d_last);
        }
        const_iterator cbegin() const {
            return begin();
        }
        const_iterator cend() const {
            return end();
        }
        reverse_iterator rbegin() {
            return reverse_iterator(end());
        }
        reverse_iterator rend() {
            return reverse_iterator(begin());
        }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }
        const_reverse_iterator rend() const{
            return const_reverse_iterator(begin());
        }
        reference operator[](size_type index) {
            return *(begin() + index);
        }
        const_reference operator[](size_type index) const {
            return *(begin() + index);
        }
        reference at(size_type index) {
            if(index >= size()){
                throw std::out_of_range("circular_buffer index is out of range");
            }
            return (*this)[index];
        }
        const_reference at(size_type index) const{
            if(index >= size()){
                throw std::out_of_range("circular_buffer index is out of range");
            }
            return (*this)[index];
        }
        reference front() {
            return *begin();
        }
        reference back() {
            return *rbegin();
        }
        const_reference front() const{
            return *begin();
        }
        const_reference back() const{
            return *rbegin();
        }
        size_type size() const {
            if(d_last >= d_first){
                return d_last - d_first;
            }else{
                return (d_end - d_first) + (d_last - d_begin);
            }
        }
        bool empty() const {
            return d_first == d_last;
        }
        bool full() const {
            return size() == capacity();
        }
        size_type reserve() const {
            return capacity() - size();
        }
        size_type capacity() const {
            return d_begin == nullptr ? 0 : (d_end - d_begin -1);
        }
        
        void set_capacity(size_type __capacity){
            if(size() > __capacity){
                this_type temp(__capacity,begin(),begin()+__capacity,d_alloc);
                swap(temp);
            }else{
                this_type temp(__capacity,begin(),end(),d_alloc);
                swap(temp);
            }
        }
        
        void resize(size_type __size, param_value_type value = value_type()){
            if(__size < size()){
                erase_end(size()-__size);
            }else if(__size > size()){
                insert(end(), __size - size(),value);
            }
        }
        
        void swap(circular_buffer< T, Alloc > & other) noexcept{
            std::swap(d_alloc,other.d_alloc);
            std::swap(d_begin,other.d_begin);
            std::swap(d_end,other.d_end);
            std::swap(d_first,other.d_first);
            std::swap(d_last,other.d_last);
        }
        void push_back(param_value_type value) {
            if(capacity()==0){
                return;
            }
            if (full()) {
                d_alloc.destroy(d_first);
                increment(d_first);
            }
            d_alloc.construct(d_last,value);
            increment(d_last);
        }
        void push_back(rvalue_type rvalue) {
            if(capacity()==0){
                return;
            }
            if (full()) {
                d_alloc.destroy(d_first);
                increment(d_first);
            }
            d_alloc.construct(d_last,std::move(rvalue));
            increment(d_last);
        }
        template <typename ... Args>
        void emplace_back(Args&& ... args){
            if(capacity()==0){
                return;
            }
            if (full()) {
                d_alloc.destroy(d_first);
                increment(d_first);
            }
            d_alloc.construct(d_last,std::forward<Args>(args)...);
            increment(d_last);
        }
        void push_front(param_value_type value){
            if(capacity()==0){
                return;
            }
            if (full()) {
                decrement(d_last);
                d_alloc.destroy(d_last);
            }
            decrement(d_first);
            d_alloc.construct(d_first,value);
        }
        void push_front(rvalue_type rvalue){
            if(capacity()==0){
                return;
            }
            if (full()) {
                decrement(d_last);
                d_alloc.destroy(d_last);
            }
            decrement(d_first);
            d_alloc.construct(d_first,std::move(rvalue));
        }
        template <typename ... Args>
        void emplace_front(Args&& ... args){
            if(capacity()==0){
                return;
            }
            if (full()) {
                decrement(d_last);
                d_alloc.destroy(d_last);
            }
            decrement(d_first);
            d_alloc.construct(d_first,std::forward<Args>(args)...);
        }
        
        void pop_back(){
            decrement(d_last);
            d_alloc.destroy(d_last);
        }
        void pop_front(){
            d_alloc.destroy(d_first);
            increment(d_first);
        }
        
        iterator insert(iterator pos, param_value_type value){
            if(pos == begin()){
                std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(end()-1),end());
                *pos = value;
                return pos;
            }
            if (full()) {
                d_alloc.destroy(d_first);
                increment(d_first);
            }
            if(pos == end()){
                d_alloc.construct(d_last,value);
            }else{
                d_alloc.construct(d_last,std::move(*rbegin()));
                std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(end()-1),end());
                *pos = value;
            }
            increment(d_last);
            return pos;
        }
        iterator insert(iterator pos, rvalue_type rvalue){
            if(pos == begin()){
                std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(end()-1),end());
                *pos = std::move(rvalue);
                return pos;
            }
            if (full()) {
                d_alloc.destroy(d_first);
                increment(d_first);
            }
            if(pos == end()){
                d_alloc.construct(d_last,std::move(rvalue));
            }else{
                d_alloc.construct(d_last,std::move(*rbegin()));
                std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(end()-1),end());
                *pos = std::move(rvalue);
            }
            increment(d_last);
            return pos;
        }
        void insert(iterator pos, size_type n, param_value_type value){
            size_type max_insert_n = capacity() - (end() - pos);
            if(n >= max_insert_n){
                for(auto iter = begin(); iter != pos; ++iter){
                    d_alloc.destroy(&*iter);
                }
                d_first = add(d_last , 1);
                std::uninitialized_fill(begin(), begin()+max_insert_n, value);
            }else{
                if(n > reserve()){
                    size_type destroy_count = n - reserve();
                    for(auto iter = begin(),end_iter = begin() + destroy_count; iter != end_iter; ++iter){
                        d_alloc.destroy(&*iter);
                    }
                    d_first = add(d_first, destroy_count);
                }
                size_type move_count = end() - pos;
                if(n >= move_count){//not overlay
                    std::uninitialized_copy(pos,  end(),  pos + n);
                    for(auto iter = pos; iter != end(); ++iter){
                        d_alloc.destroy(&*iter);
                    }
                }else{ //overlay
                    std::uninitialized_copy(end()-n,end(),end());//not overlay
                    std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(pos+(move_count-n)),end());//overlay
                    for(auto iter = pos,end_iter = pos + n; iter != end_iter; ++iter){
                        d_alloc.destroy(&*iter);
                    }
                }
                std::uninitialized_fill(pos, pos+n, value);
                d_last = add(d_last,n);
            }
        }
        template<typename InputIterator>
        void insert(iterator pos, InputIterator __begin, InputIterator __end){
            difference_type n = std::distance(__begin, __end);
            size_type max_insert_n = capacity() - (end() - pos);
            if(n >= max_insert_n){
                for(auto iter = begin(); iter != pos; ++iter){
                    d_alloc.destroy(&*iter);
                }
                d_first = add(d_last,1);
                InputIterator new_begin = __begin;
                std::advance(new_begin, n-max_insert_n);
                std::uninitialized_copy(new_begin, __end, begin());
            }else{
                if(n > reserve()){
                    size_type destroy_count = n - reserve();
                    for(auto iter = begin(),end_iter = begin() + destroy_count; iter != end_iter; ++iter){
                        d_alloc.destroy(&*iter);
                    }
                    d_first = add(d_first, destroy_count);
                }
                size_type move_count = end() - pos;
                if(n >= move_count){//not overlay
                    std::uninitialized_copy(pos,  end(),  pos + n);
                    for(auto iter = pos; iter != end(); ++iter){
                        d_alloc.destroy(&*iter);
                    }
                }else{ //overlay
                    std::uninitialized_copy(end()-n,end(),end());//not overlay
                    std::copy_backward(std::move_iterator<iterator>(pos),std::move_iterator<iterator>(pos+(move_count-n)),end());//overlay
                    for(auto iter = pos,end_iter = pos + n; iter != end_iter; ++iter){
                        d_alloc.destroy(&*iter);
                    }
                }
                std::uninitialized_copy(__begin, __end, pos);
                d_last = add(d_last,n);
            }
        }
        
        iterator erase(iterator pos){
            return erase(pos,pos+1);
        }
        iterator erase(iterator __begin, iterator __end){
            std::copy(std::move_iterator<iterator>(__end),std::move_iterator<iterator>(this->end()),__begin);
            size_type n = __end - __begin;
            for(auto iter = rbegin(),iter_end = rbegin() + n; iter != iter_end; ++iter){
                d_alloc.destroy(&*iter);
            }
            d_last = sub(d_last,n);
            return __begin;
        }
        
        void erase_begin(size_type n){
            for(auto iter = begin(),end = begin()+n; iter != end; ++iter){
                d_alloc.destroy(&*iter);
            }
            d_first = add(d_first,n);
        }
        void erase_end(size_type n){
            for(auto iter = rbegin(),end = rbegin()+n; iter != end; ++iter){
                d_alloc.destroy(&*iter);
            }
            d_last = sub(d_last,n);
        }
        void clear() {
            if(!empty()){
                for(auto iter = begin(); iter != end(); ++iter){
                    d_alloc.destroy(&*iter);
                }
                d_first = d_begin;
                d_last = d_begin;
            }
        }
    private:
        void init(size_type capacity, size_type size){
            d_begin = d_alloc.allocate(capacity + 1);
            d_end   = d_begin + capacity + 1;
            d_first = d_begin;
            d_last  = d_begin + size;
        }
        void increment(pointer & p) const{
            p = (p+1 == d_end ? d_begin : p+1);
        }
        void decrement(pointer & p) const{
            p = ( p == d_begin ? d_end -1 : p-1);
        }
        pointer add(pointer p, difference_type n) const{
            pointer ret = p;
            if(p + n >= d_end){
                ret = d_begin + (n - (d_end - p));
            }else{
                ret = p + n;
            }
            return ret;
        }
        pointer sub(pointer p, difference_type n) const{
            pointer ret = p;
            if(p - n < d_begin){
                ret = d_end - (n - (p - d_begin));
            }else{
                ret = p - n;
            }
            return ret;
        }
        pointer linearize(pointer p) const{
            if(p >= d_first){
                return p;
            }else{
                return d_end + (p - d_begin);
            }
        }
        difference_type distance(pointer p1,pointer p2) const{
            return linearize(p1) - linearize(p2);
        }
        bool less(pointer p1,pointer p2) const{
            if( (p1 >= d_first && p2 >= d_first)
               || (p1 < d_first && p2 < d_first)){
                return p1 < p2;
            }else{
                return p1 > p2;
            }
        }
    private:
        template<typename CBType>
        friend class _circular_buffer_const_iterator;
    private:
        pointer d_begin;
        pointer d_end;
        pointer d_first;
        pointer d_last;
        allocator_type d_alloc;
    };
}

