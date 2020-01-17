#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "Struct.h"
#include "Prototypes.h"
#include "Global.h"
#include "Macro.h"

// Pattern 1: shock-shock
//         2: rarefaction-shock
//         3: shock-rarefaction
//         4: rarefaction-rarefaction


int GetWavePattern( struct InitialCondition *IC )
{
  double DensLeft      = IC -> DensLeft     ;
  double VelocityLeft  = IC -> VelocityLeft ;
  double PresLeft      = IC -> PresLeft     ;
  double DensRight     = IC -> DensRight    ;
  double VelocityRight = IC -> VelocityRight;
  double PresRight     = IC -> PresRight    ;

  double SS, RS, RR;
  double V_LC, V_RC;
  bool Shock_Yes = true;
  bool Shock_No  = false;
  bool Swap_Yes = false;

  double A_Plus, A_Minus;

  // swap PresLeft and PresRight
  if ( PresLeft < PresRight )
  {
    PresLeft  = PresLeft * PresRight;
    PresRight = PresLeft / PresRight; 
    PresLeft  = PresLeft / PresRight; 

    DensLeft  = DensLeft * DensRight;
    DensRight = DensLeft / DensRight; 
    DensLeft  = DensLeft / DensRight; 

    double temp1, temp2;

	temp1 = -VelocityLeft;
	temp2 = -VelocityRight;

    VelocityLeft  = temp2;
    VelocityRight = temp1; 

    Swap_Yes = true;
  }


  // shock-shock
  double A, B, C, EnthalpyRight, Root, Engy_Temp, EngyRight;

  EnthalpyRight = Flu_Enthalpy ( PresRight, DensRight );
  EngyRight     = Flu_TotalEngy( PresRight, DensRight );

  A = 1.0 + ( Gamma_1 / Gamma ) * ( PresRight / PresLeft - 1.0 );
  B = - ( Gamma_1 / Gamma ) * ( PresRight / PresLeft - 1.0 );
  C = EnthalpyRight * ( PresRight - PresLeft ) / DensRight - SQR(EnthalpyRight);


  QuadraticSolver( A, B, C, &Root, NULL ); 

  Engy_Temp = ( Gamma / Gamma_1 ) * ( Root / (Root-1.0) ) * PresLeft - PresLeft;// eq. (4.165)

  SS = sqrt( ( PresLeft-PresRight )*( Engy_Temp-EngyRight )/( Engy_Temp+PresRight )/( EngyRight+PresLeft ) );

  // rarefaction-shock
  double DensStarLeft = DensLeft*pow(  PresRight/PresLeft, 1.0/Gamma );

  A_Plus = A_PlusFun( PresRight, DensStarLeft, PresLeft, DensLeft );

  V_LC  = ( 1.0 - A_Plus ) / ( 1.0 + A_Plus ); // eq.(4.172)

  RS = V_LC;

  // rarefaction-rarefaction

  A_Plus  = A_PlusFun ( 0.0, NAN, PresLeft,  DensLeft  );
  A_Minus = A_MinusFun( 0.0, NAN, PresRight, DensRight );

  RR = - ( A_Plus - A_Minus )/( A_Plus + A_Minus ); // put p3=p3'=0 into eq.(4.177) 

  // relative velocity  
  double RelitiveVelocity;
  int Pattern;
  RelitiveVelocity = ( VelocityLeft - VelocityRight ) / ( 1.0 - VelocityLeft*VelocityRight );


  if ( RelitiveVelocity >= SS )
  {
    Pattern = 1;
	//printf("you have shock-shock wave pattern !!\n");
  }
  else if (  RS <= RelitiveVelocity && RelitiveVelocity < SS && Swap_Yes == false )
  {
    Pattern = 2;
	//printf("you have rarefaction-shock wave pattern !!\n");
  }
  else if (  RS <= RelitiveVelocity && RelitiveVelocity < SS && Swap_Yes == true )
  {
    Pattern = 3;
	//printf("you have shock-rarefaction wave pattern !!\n");
  }
  else if ( RR <= RelitiveVelocity && RelitiveVelocity < RS )
  {
    Pattern = 4;
	//printf("you have rarefaction-rarefaction wave pattern !!\n");
  }
  else
  {
    printf("wave pattern was not found!!\n");
	exit(1);
  }

  return Pattern;

}


double Velocity_LC ( double PresStar, double DensStarLeft, double PresLeft, double DensLeft, bool Shock )
{
  double Velocity_LC;

  if ( Shock == true )
  {
     double EngyStarLeft, EngyLeft, EnthalpyStarLeft;

     EnthalpyStarLeft = TaubAdiabatic( PresLeft, DensLeft, PresStar );

	 EngyStarLeft = PresStar * ( EnthalpyStarLeft / (EnthalpyStarLeft-1.0) ) * ( Gamma / Gamma_1 ) - PresStar;

	 EngyLeft = Flu_TotalEngy( PresLeft, DensLeft );

	 // 4-velocity
     Velocity_LC  = ( PresStar     - PresLeft ) * ( EngyStarLeft - EngyLeft );
	 Velocity_LC /= ( EngyStarLeft + PresStar ) * ( EngyLeft     + PresLeft );



     if ( Velocity_LC < 0.0 )
	 {
	   printf("Velocity_LC = %e\n !!", Velocity_LC);
	   exit(1);
	 }

	 Velocity_LC  = sqrt( Velocity_LC );

  
     return Velocity_LC;
  }
  else
  {
     if ( DensStarLeft != DensStarLeft )
	 {
	   printf( "DensStarLeft should be provided!!\n" );
	   exit(1);
	 }


	 // 4-velocity
	 double A_Plus = A_PlusFun( PresStar, DensStarLeft, PresLeft, DensLeft );
     Velocity_LC = ( 1.0 - A_Plus ) / ( 2.0 * sqrt(A_Plus) );

     return Velocity_LC; 
  }
}

double Velocity_RC ( double PresStar, double DensStarRight, double PresRight, double DensRight, bool Shock )
{
  double Velocity_RC;

  if ( Shock == true )
  {
     double EngyStarRight, EngyRight, EnthalpyStarRight;

     EnthalpyStarRight = TaubAdiabatic( PresRight, DensRight, PresStar );

	 EngyStarRight = PresStar * ( EnthalpyStarRight / (EnthalpyStarRight-1.0) ) * ( Gamma / Gamma_1 ) - PresStar;

	 EngyRight = Flu_TotalEngy( PresRight, DensRight );


	 // 4-velocity
     Velocity_RC  = ( PresStar      - PresRight ) * ( EngyStarRight - EngyRight );
	 Velocity_RC /= ( EngyStarRight + PresStar  ) * ( EngyRight     + PresRight );

     if ( Velocity_RC < 0.0 )
	 {
	   printf("Velocity_RC = %e\n !!", Velocity_RC);
	   exit(1);
	 }

	 Velocity_RC  = -sqrt( Velocity_RC );

     return Velocity_RC;
  }
  else
  {
     if ( DensStarRight != DensStarRight )
	 {
	   printf( "DensStarRight should be provided!!\n" );
	   exit(1);
	 }


	 // 4-velocity
	 double A_Minus = A_MinusFun( PresStar, DensStarRight, PresRight, DensRight );;
     Velocity_RC = ( 1.0 - A_Minus ) / ( 2.0 * sqrt(A_Minus) );
  
     return Velocity_RC; 
  }
}

// A1(+) / A3(+), eq.(176) in 'Exact solution of the 1D riemann problem in Newtonian and relativistic hydrodynamics'

double A_PlusFun ( double Pres, double Dens, double PresLeft, double DensLeft )
{
    double CsLeft, Cs, Sqrt_Gamma_1, A_Plus;

	CsLeft = Flu_SoundSpeed ( PresLeft, DensLeft );
	Cs     = Flu_SoundSpeed ( Pres    , Dens     );

    Sqrt_Gamma_1 = sqrt( Gamma_1 );

	A_Plus  = ( Sqrt_Gamma_1 - Cs     ) / ( Sqrt_Gamma_1 + Cs     );
	A_Plus *= ( Sqrt_Gamma_1 + CsLeft ) / ( Sqrt_Gamma_1 - CsLeft );
    A_Plus  = pow( A_Plus, 2.0/Sqrt_Gamma_1 );

	return A_Plus;
}

// A6(-) / A4(-), eq.(179) in 'Exact solution of the 1D riemann problem in Newtonian and relativistic hydrodynamics'

double A_MinusFun ( double Pres, double Dens, double PresRight, double DensRight )
{
    double CsRight, Cs, Sqrt_Gamma_1, A_Minus;

	CsRight = Flu_SoundSpeed ( PresRight, DensRight );
	Cs      = Flu_SoundSpeed ( Pres     , Dens      );

    Sqrt_Gamma_1 = sqrt( Gamma_1 );

	A_Minus  = ( Sqrt_Gamma_1 - Cs      ) / ( Sqrt_Gamma_1 + Cs      );
	A_Minus *= ( Sqrt_Gamma_1 + CsRight ) / ( Sqrt_Gamma_1 - CsRight );
    A_Minus  = pow( A_Minus, -2.0/Sqrt_Gamma_1 );

	return A_Minus;
}


double PresFunction( double PresStar, void  *params )
{

  if ( PresStar <= 0.0 )
  {
    printf("PresStar=%e !!\n", PresStar);
	exit(1);
  }

  struct InitialCondition *IC = ( struct InitialCondition * ) params;
  double DensLeft      = IC -> DensLeft     ;
  double VelocityLeft  = IC -> VelocityLeft ;
  double PresLeft      = IC -> PresLeft     ;
  double DensRight     = IC -> DensRight    ;
  double VelocityRight = IC -> VelocityRight;
  double PresRight     = IC -> PresRight    ;
  
  bool Shock_Yes = true;
  bool Shock_No  = false;
  double V_LC, V_RC, V_LR;
  double DensStarLeft, DensStarRight;


  if ( PresStar >= MAX(PresLeft, PresRight) )
  {
    V_LC = Velocity_LC( PresStar, NAN, PresLeft,   DensLeft, Shock_Yes ); // left side of eq. (4.161)
    V_RC = Velocity_RC( PresStar, NAN, PresRight, DensRight, Shock_Yes ); // right side of eq. (4.161)

	V_LR = - V_RC * sqrt(1.0 + V_LC * V_LC) + sqrt(1.0 + V_RC * V_RC) * V_LC;
  }
  else if ( MIN(PresLeft, PresRight) <= PresStar && PresStar < MAX(PresLeft, PresRight) && PresLeft >= PresRight  )
  {
    DensStarLeft = DensLeft*pow(  PresStar/PresLeft, 1.0/Gamma );

    V_LC = Velocity_LC( PresStar, DensStarLeft, PresLeft,   DensLeft, Shock_No  ); // eq. (4.168)
    V_RC = Velocity_RC( PresStar, NAN,         PresRight,  DensRight, Shock_Yes ); // right side of eq. (4.161) 

	V_LR = - V_RC * sqrt(1.0 + V_LC * V_LC) + sqrt(1.0 + V_RC * V_RC) * V_LC;
  }
  else if ( MIN(PresLeft, PresRight) <= PresStar && PresStar < MAX(PresLeft, PresRight) && PresLeft <= PresRight )
  {
    DensStarRight = DensRight*pow(  PresStar/PresRight, 1.0/Gamma );
  
    V_LC = Velocity_LC( PresStar, NAN,          PresLeft,  DensLeft, Shock_Yes );
    V_RC = Velocity_RC( PresStar, DensStarRight, PresRight, DensRight, Shock_No  );

	V_LR = - V_RC * sqrt(1.0 + V_LC * V_LC) + sqrt(1.0 + V_RC * V_RC) * V_LC;
  }
  else if ( PresStar < MIN(PresLeft, PresRight)  )
  {
    DensStarLeft  = DensLeft *pow(  PresStar/PresLeft,  1.0/Gamma );
    DensStarRight = DensRight*pow(  PresStar/PresRight, 1.0/Gamma );

	double A_Plus, A_Minus;

	A_Plus  = A_PlusFun( PresStar,  DensStarLeft,  PresLeft,  DensLeft );
	A_Minus = A_MinusFun( PresStar, DensStarRight, PresRight, DensRight ); 

	V_LR = ( A_Minus - A_Plus ) / sqrt( 4.0*A_Minus*A_Plus );
  }

  double RelitiveVelocity;

  VelocityLeft  = VelocityLeft  / sqrt(1.0 - VelocityLeft* VelocityLeft);
  VelocityRight = VelocityRight / sqrt(1.0 - VelocityRight*VelocityRight);

  RelitiveVelocity = - VelocityRight * sqrt(1.0 + VelocityLeft *VelocityLeft )
		             +  VelocityLeft * sqrt(1.0 + VelocityRight*VelocityRight);

  return RelitiveVelocity - V_LR;
}





void QuadraticSolver( double A, double B, double C , double *PlusRoot, double *MinusRoot)
{
  double Delta;

  Delta = sqrt( B*B - 4.0*A*C );

  if ( Delta != Delta )
  {
    printf("no solutions in quadratic equation!!\n");
	exit(1);
  }

  if ( PlusRoot  != NULL )  *PlusRoot  = -2.0*C/( +B + Delta);
  if ( MinusRoot != NULL )  *MinusRoot = +2.0*C/( -B + Delta);

}
