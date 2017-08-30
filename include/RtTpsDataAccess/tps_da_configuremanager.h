//////////////////////////////////////////////////////////////////////////
/// \defgroup PluginMgr of Radio Therapy Business Unit
///  Copyright, (c) Shanghai United Imaging Healthcare Inc., 2011
///  All rights reserved.
///
///  \author  jiandong.zhou jiandong.zhou@united-imaging.com
///
///  \file    tps_configure_manager.h
///  \brief   base class of backend configure
///
///  \version 1.0
///  \date    Oct. 9, 2013
///  \{
//////////////////////////////////////////////////////////////////////////
#ifndef TPS_CONFIGURE_H
#define TPS_CONFIGURE_H

#include "tps_da_defs.h"
#include "tps_singleton.h"
#include "RtTpsFramework/tps_fw_common_enums.h"
#include "RtTpsDataAccess/tps_da_datamanagerbase.h"

TPS_BEGIN_NAMESPACE

class DoseLineTemplateItem;
class DoseLineTemplate;
class BeamTemplate;
class VOITemplate;
class OptimizerTemplate;

class RtTemplate;

class TPS_DA_DECLSPEC TpsConfigurationManager : public TpsDataManagerBase
{
public:
    TpsConfigurationManager();
    ~TpsConfigurationManager();

public:
    bool AddSystemConfig(const RtTemplate&);
    bool RemoveSystemConfig(const std::string&);
    bool GetSystemConfig(const std::string&, RtTemplate**);
    bool UpdateSystemConfig(const RtTemplate& systemConfig);
    void GetAllSystemConfig(std::vector<RtTemplate*>& allSystemConfig); 
    void GetSystemConfigFromDatatype(const int dataType, std::vector<RtTemplate*>& allSystemConfig);

    bool AddDoseLineTemplate(const DoseLineTemplate&);
    bool RemoveDoseLineTemplate(const std::string&);
    bool GetDoseLineTemplateList(std::vector<DoseLineTemplate*>&);

    bool AddVOITemplate(VOITemplate* voiTemplate);
    void GetVOITemplateList(std::vector<VOITemplate*> *voiTemplates) const;
    bool GetVOITemplate(const std::string voiTemplateUid, VOITemplate** voiTemplate) const;
    bool RemoveVOITemplate(const std::string voiTemplateUid);
    void ClearVOITemplateList();

    bool AddBeamTemplate(BeamTemplate* beamTemplate);
    void GetBeamTemplateList(std::vector<BeamTemplate*> *bt) const;
    bool GetBeamTemplate(const std::string& beamTemplateUid, BeamTemplate **beamTemplate) const;
    bool RemoveBeamTemplate(const std::string& beamTemplateUid);
    void ClearBeamTemplateList();
    std::string GetFirstBeamTemplateName() const;

    bool AddOptimizerTemplate(OptimizerTemplate* optimizerTemplate);
    void GetOptimizerTemplateList(std::vector<OptimizerTemplate*> *optTemplates) const;
    bool GetOptimizerTemplate(const std::string& templateName, OptimizerTemplate *&optm) const;
    bool RemoveOptimizerTemplate(const std::string& templateName);
    void ClearOptimizerTemplateList();

private:
    void Dispose();

private:
    std::map<std::string, DoseLineTemplate*>                       mDoseLineTemplateCollection;
    std::map<std::string, BeamTemplate*>                            mBeamTemplateCollection;
    std::map<std::string, VOITemplate*>                              mVOITemplateCollection;
    std::map<std::string, OptimizerTemplate*>                        mOptimizerTemplateCollection;

    std::map<std::string, RtTemplate*>        mSystemconfigCollection;

};

TPS_END_NAMESPACE

#endif //TPS_CONFIGURE
