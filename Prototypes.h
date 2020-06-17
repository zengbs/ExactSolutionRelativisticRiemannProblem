#ifndef __PROTOTYPE_H__
#define __PROTOTYPE_H__

#include "Struct.h"

#define TM    0
#define GAMMA 1


int GetWavePattern( struct InitialCondition *IC );

double JumpConditionForEnthalpy( double EnthalpyDown, void* params );

double Velocity_LC ( double PresStar, double DensStarLeft, double PresLeft, double DensLeft, bool Shock );

double Velocity_RC ( double PresStar, double DensStarRight, double PresRight, double DensRight, bool Shock );

double A_PlusFun ( double Temp );

double A_MinusFun ( double Temp);

double GetEnthalpyDown ( double PresUp, double DensUp, double PresDown );

double PresFunction( double PresStar, void * );

double Flu_SoundSpeed( double Temp );

double Flu_Enthalpy( double Pres, double Dens );

double Flu_TotalInternalEngy ( double Pres, double Dens );

double Enthalpy2Temperature( double Enthalpy );

double RootFinder( double(*Function)(double X, void *params) , void *params, double AbsErr, double RelErr,
			       double Guess, double LowerBound, double UpperBound );


double MassCurrent( double PresUp, double DensUp, double PresDown, double DensDown );

int GetAllInfomation( struct InitialCondition *, struct RiemannProblem  * );
void Plot( int Pattern, struct RiemannProblem *, struct PlotParams   );

double GetVelocityDown( double PresUp,   double DensUp, double ShockFrontVelocity,
                        double PresDown, double DensDown );


double GetDensDown( double PresUp, double DensUp, double PresDown  );

void GetShockVelocity( double PresUp,   double DensUp,   double V_Up, 
				    double PresDown, double DensDown,
			        double *Vs_Left, double *Vs_Right );

void QuadraticSolver( double A, double B, double C , double *PlusRoot, double *MinusRoot);

void GetHeadTailVelocity( double PresHead, double DensHead, double VelocityHead,
			              double PresTail, double DensTail, double VelocityTail,
                          double *HeadVelocity, double *TailVelocity, bool Right_Yes );

double GetDensDownRarefaction( double PresDown, double PresUp, double DensUp );

double GetVelocityDownRarefaction( double PresDown, double DensDown, double PresUp, double DensUp, double VelocityUp, bool Right_Yes );

double GetDensInFan( double Cs2, double PresHead, double DensHead );


double GetPresInFan( double DensInFan, double PresHead, double DensHead );

double GetVelocityInFan( double Cs2, double Xi, bool Right_Yes );

double GetSoundSpeedInFan ( struct Rarefaction *Rarefaction );

double TemperatureFunction ( double Temp, void *params );

double U2V(double U);

double Isentropic_Constant ( double Init_Temp, double Init_Dens );

double Isentropic_Dens2Temperature ( double Dens, double Init_Temp, double Init_Dens );

double Isentropic_Temperature2Dens ( double Temperature, double Init_Temp, double Init_Dens );

double Isentropic_Pres2Temperature ( struct Rarefaction *Rarefaction );

double Isentropic_Temperature2Pres ( double Temperature, void *params  );

double Isentropic_Pres2Dens ( struct Rarefaction *Rarefaction );

double Isentropic_Dens2Pres ( double Dens, double Init_Temp, double Init_Dens );

void RelativeVelocity( double Ua, double Ub, double *LorentzFactor_ab, double *U_ab );

double Isentropic_Dens2Velocity ( double DensDown, struct Rarefaction *upstream );
#endif
