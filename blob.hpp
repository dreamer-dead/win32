#if !defined HDR_BLOB_HPP
#define HDR_BLOB_HPP

namespace w32 
{
	struct blob
	{
		unsigned long size;
		void * data;

		blob( unsigned long s, void * p ) : size(s), data(p) {}
	};
}

#endif /* HDR_BLOB_HPP */
