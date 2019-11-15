#include "blockdata.h"
#include "ui_blockdata.h"

BlockData::BlockData(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BlockData)
{
    ui->setupUi(this);
    this->setWindowTitle("Block Data");
}

BlockData::~BlockData()
{
    delete ui;
}

void BlockData::setBlockData( QString block,
                              int occ,
                              int total,
                              int csenior,
                              int senior,
                              int sm_taken,
                              int dm_taken,
                              int sf_taken,
                              int df_taken,
                              int sm_empty,
                              int dm_empty,
                              int sf_empty,
                              int df_empty,
                              int sm_total,
                              int dm_total,
                              int sf_total,
                              int df_total,
                              QStringList nationality_text,
                              QStringList available_room)

{

    if(block == "AB"){
        ui->txt_occ_ab              ->setText( QString::number(occ));
        ui->txt_total_ab            ->setText( QString::number(total));
        ui->txt_csenior_ab          ->setText( QString::number(csenior));
        ui->txt_senior_ab           ->setText( QString::number(senior));
        ui->txt_sm_taken_ab         ->setText( QString::number(sm_taken));
        ui->txt_dm_taken_ab         ->setText( QString::number(dm_taken));
        ui->txt_sf_taken_ab         ->setText( QString::number(sf_taken));
        ui->txt_df_taken_ab         ->setText( QString::number(df_taken));
        ui->txt_sm_emp_ab           ->setText( QString::number(sm_empty));
        ui->txt_dm_emp_ab           ->setText( QString::number(dm_empty));
        ui->txt_sf_emp_ab           ->setText( QString::number(sf_empty));
        ui->txt_df_emp_ab           ->setText( QString::number(df_empty));
        ui->txt_sm_total_ab         ->setText( QString::number(sm_total));
        ui->txt_dm_total_ab         ->setText( QString::number(dm_total));
        ui->txt_sf_total_ab         ->setText( QString::number(sf_total));
        ui->txt_df_total_ab         ->setText( QString::number(df_total));
        ui->txt_natperfloor_ab      ->setText( nationality_text.join('\n'));
        ui->txt_availableroom_ab    ->setText( available_room.join('\n'));
    }
    else if(block == "CD"){
        ui->txt_occ_cd              ->setText( QString::number(occ));
        ui->txt_total_cd            ->setText( QString::number(total));
        ui->txt_csenior_cd          ->setText( QString::number(csenior));
        ui->txt_senior_cd           ->setText( QString::number(senior));
        ui->txt_sm_taken_cd         ->setText( QString::number(sm_taken));
        ui->txt_dm_taken_cd         ->setText( QString::number(dm_taken));
        ui->txt_sf_taken_cd         ->setText( QString::number(sf_taken));
        ui->txt_df_taken_cd         ->setText( QString::number(df_taken));
        ui->txt_sm_emp_cd           ->setText( QString::number(sm_empty));
        ui->txt_dm_emp_cd           ->setText( QString::number(dm_empty));
        ui->txt_sf_emp_cd           ->setText( QString::number(sf_empty));
        ui->txt_df_emp_cd           ->setText( QString::number(df_empty));
        ui->txt_sm_total_cd         ->setText( QString::number(sm_total));
        ui->txt_dm_total_cd         ->setText( QString::number(dm_total));
        ui->txt_sf_total_cd         ->setText( QString::number(sf_total));
        ui->txt_df_total_cd         ->setText( QString::number(df_total));
        ui->txt_natperfloor_cd      ->setText( nationality_text.join('\n'));
        ui->txt_availableroom_cd    ->setText( available_room.join('\n'));
    }
    else if(block == "E"){
        ui->txt_occ_e              ->setText( QString::number(occ));
        ui->txt_total_e            ->setText( QString::number(total));
        ui->txt_csenior_e          ->setText( QString::number(csenior));
        ui->txt_senior_e           ->setText( QString::number(senior));
        ui->txt_sm_taken_e         ->setText( QString::number(sm_taken));
        ui->txt_dm_taken_e         ->setText( QString::number(dm_taken));
        ui->txt_sf_taken_e         ->setText( QString::number(sf_taken));
        ui->txt_df_taken_e         ->setText( QString::number(df_taken));
        ui->txt_sm_emp_e           ->setText( QString::number(sm_empty));
        ui->txt_dm_emp_e           ->setText( QString::number(dm_empty));
        ui->txt_sf_emp_e           ->setText( QString::number(sf_empty));
        ui->txt_df_emp_e           ->setText( QString::number(df_empty));
        ui->txt_sm_total_e         ->setText( QString::number(sm_total));
        ui->txt_dm_total_e         ->setText( QString::number(dm_total));
        ui->txt_sf_total_e         ->setText( QString::number(sf_total));
        ui->txt_df_total_e         ->setText( QString::number(df_total));
        ui->txt_natperfloor_e      ->setText( nationality_text.join('\n'));
        ui->txt_availableroom_e    ->setText( available_room.join('\n'));
    }
    else if(block == "F"){
        ui->txt_occ_f              ->setText( QString::number(occ));
        ui->txt_total_f            ->setText( QString::number(total));
        ui->txt_csenior_f          ->setText( QString::number(csenior));
        ui->txt_senior_f           ->setText( QString::number(senior));
        ui->txt_sm_taken_f         ->setText( QString::number(sm_taken));
        ui->txt_dm_taken_f         ->setText( QString::number(dm_taken));
        ui->txt_sf_taken_f         ->setText( QString::number(sf_taken));
        ui->txt_df_taken_f         ->setText( QString::number(df_taken));
        ui->txt_sm_emp_f           ->setText( QString::number(sm_empty));
        ui->txt_dm_emp_f           ->setText( QString::number(dm_empty));
        ui->txt_sf_emp_f           ->setText( QString::number(sf_empty));
        ui->txt_df_emp_f           ->setText( QString::number(df_empty));
        ui->txt_sm_total_f         ->setText( QString::number(sm_total));
        ui->txt_dm_total_f         ->setText( QString::number(dm_total));
        ui->txt_sf_total_f         ->setText( QString::number(sf_total));
        ui->txt_df_total_f         ->setText( QString::number(df_total));
        ui->txt_natperfloor_f      ->setText( nationality_text.join('\n'));
        ui->txt_availableroom_f    ->setText( available_room.join('\n'));
    }
    else if(block == "GH"){
        ui->txt_occ_gh              ->setText( QString::number(occ));
        ui->txt_total_gh            ->setText( QString::number(total));
        ui->txt_csenior_gh          ->setText( QString::number(csenior));
        ui->txt_senior_gh           ->setText( QString::number(senior));
        ui->txt_sm_taken_gh         ->setText( QString::number(sm_taken));
        ui->txt_dm_taken_gh         ->setText( QString::number(dm_taken));
        ui->txt_sf_taken_gh         ->setText( QString::number(sf_taken));
        ui->txt_df_taken_gh         ->setText( QString::number(df_taken));
        ui->txt_sm_emp_gh           ->setText( QString::number(sm_empty));
        ui->txt_dm_emp_gh           ->setText( QString::number(dm_empty));
        ui->txt_sf_emp_gh           ->setText( QString::number(sf_empty));
        ui->txt_df_emp_gh           ->setText( QString::number(df_empty));
        ui->txt_sm_total_gh         ->setText( QString::number(sm_total));
        ui->txt_dm_total_gh         ->setText( QString::number(dm_total));
        ui->txt_sf_total_gh         ->setText( QString::number(sf_total));
        ui->txt_df_total_gh         ->setText( QString::number(df_total));
        ui->txt_natperfloor_gh      ->setText( nationality_text.join('\n'));
        ui->txt_availableroom_gh    ->setText( available_room.join('\n'));
    }

}
