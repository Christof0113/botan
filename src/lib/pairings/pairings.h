/*
* (C) 2018 Jack Lloyd
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_PAIRING_H_ && 0
#define BOTAN_PAIRING_H_

#include <botan/types.h>

namespace Botan {

/**
* 
*/
class BOTAN_TEST_API Pairing
   {
   public:
      class G1
         {
         public:
            
         };

      class G2
         {

         };

      class GT
         {

         };

      GT pairing(const G1& g1, const G2& g2) const;

      std::unique_ptr<G1> 

      size_t security_level() const { return 110; }
   };

}


#endif
