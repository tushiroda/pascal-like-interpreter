#include <iostream>
#include "val.h"

Value Value::operator+(const Value &op) const
{
   Value output = Value();

   if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal()))
   {
      // real exists
      if (this->IsReal() || op.IsReal())
      {
         // both are reals
         if (this->IsReal() && op.IsReal())
         {
            output = Value(this->GetReal() + op.GetReal());
         }

         // left is real, other isnt
         else if (this->IsReal())
         {
            output = Value(this->GetReal() + op.GetInt());
         }

         else if (op.IsReal())
         {
            output = Value(this->GetInt() + op.GetReal());
         }
      }

      // both ints
      else
      {
         output = Value(this->GetInt() + op.GetInt());
      }
   }
   return output;
}

Value Value::operator-(const Value &op) const
{
   Value output = Value();

   if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal()))
   {
      // real exists
      if (this->IsReal() || op.IsReal())
      {
         // both are reals
         if (this->IsReal() && op.IsReal())
         {
            output = Value(this->GetReal() - op.GetReal());
         }

         // left is real, other isnt
         else if (this->IsReal())
         {
            output = Value(this->GetReal() - op.GetInt());
         }

         else if (op.IsReal())
         {
            output = Value(this->GetInt() - op.GetReal());
         }
      }

      // both ints
      else
      {
         output = Value(this->GetInt() - op.GetInt());
      }
   }
   return output;
}

Value Value::operator*(const Value &op) const
{
   Value output = Value();

   if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal()))
   {
      // real exists
      if (this->IsReal() || op.IsReal())
      {
         // both are reals
         if (this->IsReal() && op.IsReal())
         {
            output = Value(this->GetReal() * op.GetReal());
         }

         // left is real, other isnt
         else if (this->IsReal())
         {
            output = Value(this->GetReal() * op.GetInt());
         }

         else if (op.IsReal())
         {
            output = Value(this->GetInt() * op.GetReal());
         }
      }

      // both ints
      else
      {
         output = Value(this->GetInt() * op.GetInt());
      }
   }

   return output;
}

Value Value::operator/(const Value &op) const
{
   Value output = Value();

   if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal()))
   {
      // real exists
      if (this->IsReal() || op.IsReal())
      {
         // both are reals
         if (this->IsReal() && op.IsReal())
         {
            output = Value(this->GetReal() / op.GetReal());
         }

         // left is real, other isnt
         else if (this->IsReal())
         {
            output = Value(this->GetReal() / op.GetInt());
         }

         else if (op.IsReal())
         {
            output = Value(this->GetInt() / op.GetReal());
         }
      }

      // both ints
      else
      {
         output = Value(this->GetInt() / op.GetInt());
      }
   }

   return output;
}

Value Value::operator%(const Value &oper) const
{
   Value output = Value();

   if (this->IsInt() && oper.IsInt())
   {
      output = Value((double)(this->GetInt() % oper.GetInt()));
   }

   return output;
}

Value Value::operator==(const Value &op) const
{
   Value output = Value();

   if (((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) || this->GetType() == op.GetType())
   {
      if (op.IsInt())
         output = Value(op.Itemp == this->Itemp);
      else if (op.IsString())
         output = Value(op.Stemp == this->Stemp);
      else if (op.IsReal())
         output = Value(op.Rtemp == this->Rtemp);
      else if (op.IsBool())
         output = Value(op.Btemp == this->Btemp);
   }

   return output;
}

Value Value::operator>(const Value &op) const
{
   Value output = Value();

   if (((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) || this->GetType() == op.GetType())
   {
      if (op.IsInt())
         output = Value(this->Itemp > op.Itemp);
      else if (op.IsString())
         output = Value(this->Stemp > op.Stemp);
      else if (op.IsReal())
         output = Value(this->Rtemp > op.Rtemp);
      else if (op.IsBool())
         output = Value(this->Btemp > op.Btemp);
   }

   return output;
}

Value Value::operator<(const Value &op) const
{
   Value output = Value();

   if (((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal())) || this->GetType() == op.GetType())
   {
      if (op.IsInt())
         output = Value(this->Itemp < op.Itemp);
      else if (op.IsString())
         output = Value(this->Stemp < op.Stemp);
      else if (op.IsReal())
         output = Value(this->Rtemp < op.Rtemp);
      else if (op.IsBool())
         output = Value(this->Btemp < op.Btemp);
   }

   return output;
}

Value Value::div(const Value &op) const
{
   Value output = Value();

   if ((this->IsInt() || this->IsReal()) && (op.IsInt() || op.IsReal()))
   {
      // real exists
      if (this->IsReal() || op.IsReal())
      {
         // both are reals
         if (this->IsReal() && op.IsReal())
         {
            output = Value(static_cast<int>(this->GetReal() / op.GetReal()));
         }

         // left is real, other isnt
         else if (this->IsReal())
         {
            output = Value(static_cast<int>(this->GetReal() / op.GetInt()));
         }

         else if (op.IsReal())
         {
            output = Value(static_cast<int>(this->GetInt() / op.GetReal()));
         }
      }

      // both ints
      else
      {
         output = Value(this->GetInt() / op.GetInt());
      }
   }

   return output;
}

Value Value::operator&&(const Value &oper) const
{
   Value output = Value();

   if (this->IsBool() && oper.IsBool())
   {
      output = Value(this->GetBool() && oper.GetBool());
   }

   return output;
}

Value Value::operator||(const Value &oper) const
{
   Value output = Value();

   if (this->IsBool() && oper.IsBool())
   {
      output = Value(this->GetBool() || oper.GetBool());
   }

   return output;
}

Value Value::operator!() const
{
   Value output;

   if (this->IsBool())
   {
      output = Value(!this->GetBool());
   }
   // Not sure about this part. might only be logical?
   //  else
   //     output = Value(0);

   return output;
}
