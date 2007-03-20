
/*
 Copyright (C) 2007 Cristina Duminuco
 Copyright (C) 2006, 2007 Ferdinando Ametrano
 Copyright (C) 2006 Eric Ehlers
 Copyright (C) 2006 Giorgio Facchinetti
 Copyright (C) 2005 Aurelien Chanudet

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it under the
 terms of the QuantLib license.  You should have received a copy of the
 license along with this program; if not, please email quantlib-dev@lists.sf.net
 The license is also available online at http://quantlib.org/html/license.html

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
    #include <qlo/config.hpp>
#endif

#include <qlo/couponvectors.hpp>
#include <qlo/termstructures.hpp>
#include <qlo/typefactory.hpp>

#include <ql/cashflow.hpp>
#include <ql/CashFlows/cashflowvectors.hpp>
#include <ql/CashFlows/simplecashflow.hpp>
#include <ql/CashFlows/cmscoupon.hpp>
#include <ql/CashFlows/analysis.hpp>

using QuantLib::earlier_than;
using QuantLib::CashFlow;

namespace QuantLibAddin {

    QuantLib::Date Leg::startDate() const {
        return QuantLib::CashFlows::startDate(leg_);
    }

    QuantLib::Date Leg::maturityDate() const {
        return QuantLib::CashFlows::maturityDate(leg_);
    }

    QuantLib::Real Leg::npv(const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS) const {
        return QuantLib::CashFlows::npv(leg_, hYTS);
    }

    QuantLib::Real Leg::bps(const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS) const {
        return QuantLib::CashFlows::bps(leg_, hYTS);
    }

    QuantLib::Rate Leg::atmRate(const QuantLib::Handle<QuantLib::YieldTermStructure>& hYTS) const {
        return QuantLib::CashFlows::atmRate(leg_, hYTS);
    }

    std::vector<std::vector<boost::any> > Leg::analysis() const {
        return flowAnalysis(leg_);
    }

    void Leg::setPricer(
            const boost::shared_ptr<QuantLib::FloatingRateCouponPricer>& pricer) {
        return QuantLib::CashFlows::setPricer(leg_,pricer);
    }

    void Leg::setPricers(
            const std::vector<boost::shared_ptr<QuantLib::FloatingRateCouponPricer> >& pricers) {
        return QuantLib::CashFlows::setPricers(leg_,pricers);
    }

    const QuantLib::Leg& Leg::getQuantLibLeg() {
        return leg_;
    }

    MultiPhaseLeg::MultiPhaseLeg(
                    const std::vector<boost::shared_ptr<Leg> >& streams,
                    bool toBeSorted) {
        for (QuantLib::Size i=0; i<streams.size(); ++i) {
            const QuantLib::Leg& leg = streams[i]->getQuantLibLeg();
            leg_.insert(leg_.end(), leg.begin(), leg.end());
        }
        if (toBeSorted)
            std::sort(leg_.begin(), leg_.end(),
                      earlier_than<boost::shared_ptr<CashFlow> >());
    };

    SimpleCashFlowVector::SimpleCashFlowVector(const std::vector<QuantLib::Real>& amounts,
                                               const std::vector<QuantLib::Date>& dates)
    {
        for (QuantLib::Size i=0; i < amounts.size(); ++i) {
            leg_.push_back(boost::shared_ptr<CashFlow>(
                new QuantLib::SimpleCashFlow(amounts[i],dates[i])));
        }
    }

    FixedRateLeg::FixedRateLeg(
                    QuantLib::BusinessDayConvention paymentConvention,
                    const std::vector<QuantLib::Real>& nominals,
                    const boost::shared_ptr<QuantLib::Schedule>& schedule,
                    const std::vector<QuantLib::Rate>& couponRates,
                    const QuantLib::DayCounter& paymentDayCounter) {
        leg_ = QuantLib::FixedRateLeg(nominals,
                                      *schedule,
                                      couponRates,
                                      paymentDayCounter,
                                      paymentConvention);
    }

    IborLeg::IborLeg(
                    QuantLib::BusinessDayConvention paymentConvention,
                    const std::vector<QuantLib::Real>& nominals,
                    const boost::shared_ptr<QuantLib::Schedule>& schedule,
                    QuantLib::Natural fixingDays,
                    bool isInArrears,
                    const QuantLib::DayCounter& paymentDayCounter,
                    const std::vector<QuantLib::Rate>& floors,
                    const std::vector<QuantLib::Real>& gearings,
                    const boost::shared_ptr<QuantLib::IborIndex>& index,
                    const std::vector<QuantLib::Spread>& spreads,
                    const std::vector<QuantLib::Rate>& caps) {

        leg_ = QuantLib::IborLeg(nominals,
                                 *schedule,
                                 index,
                                 paymentDayCounter,
                                 paymentConvention,
                                 fixingDays,
                                 gearings, spreads,
                                 caps, floors,
                                 isInArrears);
    }

    IborCouponPricer::IborCouponPricer(
            const QuantLib::Handle<QuantLib::CapletVolatilityStructure>& v,
            const std::string& typeOfIborCouponPricer) {
        libraryObject_ = Create<boost::shared_ptr<QuantLib::IborCouponPricer> >()
            (typeOfIborCouponPricer, v);
    }

    CmsLeg::CmsLeg(
                    QuantLib::BusinessDayConvention paymentConvention,
                    const std::vector<QuantLib::Real>& nominals,
                    const boost::shared_ptr<QuantLib::Schedule>& schedule,
                    QuantLib::Natural fixingDays,
                    bool isInArrears,
                    const QuantLib::DayCounter& paymentDayCounter,
                    const std::vector<QuantLib::Rate>& floors,
                    const std::vector<QuantLib::Real>& gearings,
                    const boost::shared_ptr<QuantLib::SwapIndex>& index,
                    const std::vector<QuantLib::Spread>& spreads,
                    const std::vector<QuantLib::Rate>& caps) {
        leg_ = QuantLib::CmsLeg(nominals,
                                *schedule,
                                index,
                                paymentDayCounter,
                                paymentConvention,
                                fixingDays,
                                gearings, spreads,
                                caps, floors,
                                isInArrears);
    }

    CmsZeroLeg::CmsZeroLeg(
                    QuantLib::BusinessDayConvention paymentConvention,
                    const std::vector<QuantLib::Real>& nominals,
                    const boost::shared_ptr<QuantLib::Schedule>& schedule,
                    QuantLib::Natural fixingDays,
                    bool isInArrears,
                    const QuantLib::DayCounter& paymentDayCounter,
                    const std::vector<QuantLib::Rate>& floors,
                    const std::vector<QuantLib::Real>& gearings,
                    const boost::shared_ptr<QuantLib::SwapIndex>& index,
                    const std::vector<QuantLib::Spread>& spreads,
                    const std::vector<QuantLib::Rate>& caps) {
        leg_ = QuantLib::CmsZeroLeg(nominals,
                                    *schedule,
                                    index,
                                    paymentDayCounter,
                                    paymentConvention,
                                    fixingDays,
                                    gearings, spreads,
                                    caps, floors);
    }

    CmsCouponPricer::CmsCouponPricer(
            const QuantLib::Handle<QuantLib::SwaptionVolatilityStructure>& v,
            const std::string& typeOfCmsCouponPricer,
            QuantLib::GFunctionFactory::ModelOfYieldCurve modelOfYieldCurve,
            const QuantLib::Handle<QuantLib::Quote>& meanReversion) {
        libraryObject_ = Create<boost::shared_ptr<QuantLib::CmsCouponPricer> >()
            (typeOfCmsCouponPricer, v, modelOfYieldCurve, meanReversion);
    }


	ConundrumPricerByNumericalIntegration::ConundrumPricerByNumericalIntegration(
			const QuantLib::Handle<QuantLib::SwaptionVolatilityStructure>& swaptionVol,
            QuantLib::GFunctionFactory::ModelOfYieldCurve modelOfYieldCurve,
			const QuantLib::Handle<QuantLib::Quote>& meanReversion,
            QuantLib::Rate lowerLimit,
            QuantLib::Rate upperLimit,
			QuantLib::Real precision,
			QuantLib::Real numberOfStdDeviationsForUpperLimit){
				/*QuantLib::GFunctionFactory::ModelOfYieldCurve modelOfYieldCurve
					= QuantLib::GFunctionFactory::ModelOfYieldCurve(2);*/
				libraryObject_ = 
					boost::shared_ptr<QuantLib::ConundrumPricerByNumericalIntegration> (new
					QuantLib::ConundrumPricerByNumericalIntegration(swaptionVol,
                                                                    modelOfYieldCurve,
																	meanReversion,
																	lowerLimit,
																	upperLimit,
																	precision,
                                                                    numberOfStdDeviationsForUpperLimit));
	};
}
