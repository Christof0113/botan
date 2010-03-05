/*
* Nyberg-Rueppel
* (C) 1999-2010 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/nr.h>
#include <botan/numthry.h>
#include <botan/keypair.h>
#include <botan/look_pk.h>

namespace Botan {

NR_PublicKey::NR_PublicKey(const AlgorithmIdentifier& alg_id,
                           const MemoryRegion<byte>& key_bits) :
   DL_Scheme_PublicKey(alg_id, key_bits, DL_Group::ANSI_X9_57)
   {
   }

/*
* NR_PublicKey Constructor
*/
NR_PublicKey::NR_PublicKey(const DL_Group& grp, const BigInt& y1)
   {
   group = grp;
   y = y1;
   }

/*
* Create a NR private key
*/
NR_PrivateKey::NR_PrivateKey(RandomNumberGenerator& rng,
                             const DL_Group& grp,
                             const BigInt& x_arg)
   {
   group = grp;
   x = x_arg;

   if(x == 0)
      x = BigInt::random_integer(rng, 2, group_q() - 1);

   y = power_mod(group_g(), x, group_p());

   if(x_arg == 0)
      gen_check(rng);
   else
      load_check(rng);
   }

NR_PrivateKey::NR_PrivateKey(const AlgorithmIdentifier& alg_id,
                             const MemoryRegion<byte>& key_bits,
                             RandomNumberGenerator& rng) :
   DL_Scheme_PrivateKey(alg_id, key_bits, DL_Group::ANSI_X9_57)
   {
   y = power_mod(group_g(), x, group_p());

   load_check(rng);
   }

/*
* Check Private Nyberg-Rueppel Parameters
*/
bool NR_PrivateKey::check_key(RandomNumberGenerator& rng, bool strong) const
   {
   if(!DL_Scheme_PrivateKey::check_key(rng, strong) || x >= group_q())
      return false;

   if(!strong)
      return true;

   try
      {
      KeyPair::check_key(rng,
                         get_pk_signer(*this, "EMSA1(SHA-1)"),
                         get_pk_verifier(*this, "EMSA1(SHA-1)")
         );
      }
   catch(Self_Test_Failure)
      {
      return false;
      }

   return true;
   }

NR_Signature_Operation::NR_Signature_Operation(const NR_PrivateKey& nr) :
   q(nr.group_q()),
   x(nr.get_x()),
   powermod_g_p(nr.group_g(), nr.group_p()),
   mod_q(nr.group_q())
   {
   }

SecureVector<byte>
NR_Signature_Operation::sign(const byte msg[], u32bit msg_len,
                             RandomNumberGenerator& rng) const
   {
   rng.add_entropy(msg, msg_len);

   BigInt k;
   do
      k.randomize(rng, q.bits());
   while(k >= q);

   BigInt f(msg, msg_len);

   if(f >= q)
      throw Invalid_Argument("NR_Signature_Operation: Input is out of range");

   BigInt c = mod_q.reduce(powermod_g_p(k) + f);
   if(c.is_zero())
      throw Internal_Error("NR_Signature_Operation: c was zero");
   BigInt d = mod_q.reduce(k - x * c);

   SecureVector<byte> output(2*q.bytes());
   c.binary_encode(output + (output.size() / 2 - c.bytes()));
   d.binary_encode(output + (output.size() - d.bytes()));
   return output;
   }

NR_Verification_Operation::NR_Verification_Operation(const NR_PublicKey& nr) :
   q(nr.group_q()), y(nr.get_y())
   {
   powermod_g_p = Fixed_Base_Power_Mod(nr.group_g(), nr.group_p());
   powermod_y_p = Fixed_Base_Power_Mod(y, nr.group_p());
   mod_p = Modular_Reducer(nr.group_p());
   mod_q = Modular_Reducer(nr.group_q());
   }

SecureVector<byte>
NR_Verification_Operation::verify_mr(const byte msg[], u32bit msg_len) const
   {
   const BigInt& q = mod_q.get_modulus();

   if(msg_len != 2*q.bytes())
      return false;

   BigInt c(msg, q.bytes());
   BigInt d(msg + q.bytes(), q.bytes());

   if(c.is_zero() || c >= q || d >= q)
      throw Invalid_Argument("NR verification: Invalid signature");

   BigInt i = mod_p.multiply(powermod_g_p(d), powermod_y_p(c));
   return BigInt::encode(mod_q.reduce(c - i));
   }

}
