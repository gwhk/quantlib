/*
 Copyright (C) 2001, 2002 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email ferdinando@ametrano.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/
/*! \file blackkarasinski.hpp
    \brief Black-Karasinski model

    \fullpath
    ql/InterestRateModelling/OneFactorModels/%blackkarasinski.hpp
*/

// $Id$

#ifndef quantlib_one_factor_models_black_karasinski_h
#define quantlib_one_factor_models_black_karasinski_h

#include <ql/InterestRateModelling/onefactormodel.hpp>

namespace QuantLib {

    namespace InterestRateModelling {

        class GeneralBlackKarasinski : public OneFactorModel {
          public:
            GeneralBlackKarasinski(
                const Parameter& a,
                const Parameter& sigma,
                const RelinkableHandle<TermStructure>& termStructure) 
            : OneFactorModel(3, termStructure), 
              a_(parameters_[0]), sigma_(parameters_[1]), f_(parameters_[2]) {
                a_ = a;
                sigma_ = sigma;
            }
            virtual ~GeneralBlackKarasinski() {}

            virtual Handle<ShortRateProcess> process() const {
                return Handle<ShortRateProcess>(
                    new Process(f_, a_, sigma_));
            }

            virtual Handle<Lattices::Tree> tree(const TimeGrid& grid) const {
                return Handle<Lattices::Tree>(
                    new OwnTrinomialTree(process(), f_.implementation(), grid));
            }

          protected:
            virtual void generateParameters() {
                f_ = TermStructureFittingParameter(termStructure());
            }

            Parameter& a_;
            Parameter& sigma_;
            Parameter& f_;
          private:
            class Process : public OrnsteinUhlenbeckProcess {
              public:
                Process(const Parameter& fitting,
                        const Parameter& speed,
                        const Parameter& volatility)
                : OrnsteinUhlenbeckProcess(speed, volatility),
                  fitting_(fitting) {}
                virtual double variable(Time t, Rate r) const {
                    return QL_LOG(r) - fitting_(t);
                }

                virtual double shortRate(Time t, double x) const {
                    return QL_EXP(x + fitting_(t));
                }
              private:
                Parameter fitting_;
            };

        };

        class BlackKarasinski : public GeneralBlackKarasinski {
          public:
            BlackKarasinski(
                const RelinkableHandle<TermStructure>& termStructure)
            : GeneralBlackKarasinski(ConstantParameter(0.1), 
                                     ConstantParameter(0.1), 
                                     termStructure) {}
            virtual ~BlackKarasinski() {}
        };

    }

}

#endif
