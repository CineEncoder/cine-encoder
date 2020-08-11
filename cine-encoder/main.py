# -*- coding: utf-8 -*-

import sys
import ui_main
import ui_taskcomplete
import ui_donate
import ui_about
import files_rc

import os
import math
import multiprocessing
import time
import webbrowser
from PyQt5 import QtWidgets
from PyQt5.QtCore import QProcess
from pymediainfo import MediaInfo


# -------------------------------------------- Main program ----------------------------------------------------------

class ExampleApp(QtWidgets.QMainWindow, ui_main.Ui_MainWindow):
    def __init__(self, parent=None):  # init ui_main
        super().__init__(parent)
        self.setupUi(self)  # init design

# ------------------------------------------- Main Menu Buttons ------------------------------------------------------

        self.actionAbout.clicked.connect(self.about)
        self.actionDonate.clicked.connect(self.donate)
        self.turnLeftStack_1.clicked.connect(lambda: self.stackedWidget_1.setCurrentIndex(0))
        self.turnRightStack_1.clicked.connect(lambda: self.stackedWidget_1.setCurrentIndex(1))
        self.turnLeftStack_2.clicked.connect(self.turnStackLeft)
        self.turnRightStack_2.clicked.connect(self.turnStackRight)

# ---------------------------------------- Status and Buttons ---------------------------------------------------------

        #self.label_53.hide() # (add to ui_main)
        #self.label_54.hide() # (add to ui_main)
        #self.label_55.hide() # (add to ui_main)
        #self.progressBar.hide() # (add to ui_main)
        self.pushButton_1.clicked.connect(self.open_file)  # execute open_file
        self.pushButton_2.clicked.connect(self.save_file)  # execute save_file
        self.pushButton_3.clicked.connect(self.make_preset)  # make preset
        self.pushButton_4.clicked.connect(self.stop)  # make preset
        self.comboBox_1.currentTextChanged.connect(self.settings_menu)
        self.comboBox_4.currentTextChanged.connect(self.settings_menu_mode)
        self.comboBox_12.currentTextChanged.connect(self.settings_menu_audio)
        self.comboBox_15.currentTextChanged.connect(self.settings_hdr)

    def turnStackLeft(self):
        a = self.stackedWidget_2.currentIndex()
        if a != 0:
            self.stackedWidget_2.setCurrentIndex(a-1)
        else:
            self.stackedWidget_2.setCurrentIndex(0)

    def turnStackRight(self):
        a = self.stackedWidget_2.currentIndex()
        if a != 3:
            self.stackedWidget_2.setCurrentIndex(a+1)
        else:
            self.stackedWidget_2.setCurrentIndex(3)

# ----------------------------------------------- Dialogs -------------------------------------------------------------

    def open_file(self):
        global hdr1, hdr2, hdr3, hdr4, hdr5, hdr6, hdr7, hdr8, hdr9, hdr10, hdr11
        self.lineEdit_1.clear()
        self.lineEdit_2.clear()
        self.lineEdit_3.clear()
        self.lineEdit_4.clear()
        self.lineEdit_5.clear()
        self.lineEdit_6.clear()
        self.lineEdit_7.clear()
        self.lineEdit_8.clear()
        self.comboBox_9.clear()
        self.comboBox_10.clear()
        self.comboBox_11.clear()
        self.comboBox_15.clear()
        self.comboBox_16.clear()
        self.label_format.setText("n/a")
        self.label_duration.setText("n/a")
        self.label_resolution.setText("n/a")
        self.label_subsampling.setText("n/a")
        self.label_bitrate.setText("n/a")
        self.label_fps.setText("n/a")
        self.label_aspect_ratio.setText("n/a")
        self.label_color_space.setText("n/a")
        self.label_bit_depth.setText("n/a")
        self.label_audio_1.clear()
        self.label_audio_2.clear()
        self.label_audio_3.clear()
        self.label_audio_4.clear()
        self.label_audio_5.clear()
        self.label_audio_6.clear()
        self.label_audio_7.clear()
        self.label_audio_8.clear()
        self.label_audio_9.clear()
        self.label_53.hide()
        self.label_54.hide()
        self.label_55.hide()
        self.progressBar.hide()
        try:
            file_name_open = QtWidgets.QFileDialog.getOpenFileName(self, "Open File", "Untitled", ("Video Files: *.avi, "
                                                                                                   "*m2ts, *.m4v, *.mkv, "
                                                                                                   "*.mov, *.mp4, *.mpeg, "
                                                                                                   "*.mpg, *.mxf, *.ts, "
                                                                                                   "*.webm (*.avi *.m2ts "
                                                                                                   "*.m4v *.mkv *.mov "
                                                                                                   "*.mp4 *.mpeg *.mpg "
                                                                                                   "*.mxf *.ts *.webm)"))
            file_name = file_name_open[0]
            self.lineEdit_1.setText(file_name)  # Open filename to listEdit_1
        except:
            pass
        try:
            media_info = MediaInfo.parse(self.lineEdit_1.text())
            fmt = str(media_info.tracks[1].format)
            dur = 0.001*round(float(media_info.tracks[0].duration))
            h = math.trunc(dur / 3600)
            m = math.trunc((dur - (h * 3600)) / 60)
            s = math.trunc(dur - (h * 3600) - (m * 60))
            chroma_subsampling = str(media_info.tracks[1].chroma_subsampling)
            bit_rate = str(round(media_info.tracks[0].overall_bit_rate/1000000, 1))
            aspect_ratio = str(media_info.tracks[1].display_aspect_ratio)
            color_space = str(media_info.tracks[1].color_space)
            bit_depth = str(media_info.tracks[1].bit_depth)
            width = str(media_info.tracks[1].width)
            height = str(media_info.tracks[1].height)
            fps = str(media_info.tracks[1].frame_rate)
            color_range = str(media_info.tracks[1].color_range)
            color_primaries = str(media_info.tracks[1].color_primaries)
            matrix_coefficients = str(media_info.tracks[1].matrix_coefficients)
            transfer_characteristics = str(media_info.tracks[1].transfer_characteristics)
            mastering_display_luminance = str(media_info.tracks[1].mastering_display_luminance).replace('min: ', '').replace('max: ', '').replace(' cd/m2', '')
            min_luminance = mastering_display_luminance.split(', ')[0]
            max_luminance = mastering_display_luminance.replace(min_luminance, '').replace(', ', '')
            maxCll = str(media_info.tracks[1].maximum_content_light_level)
            maxFall = str(media_info.tracks[1].maximum_frameaverage_light_level)
            mastering_display_color_primaries = str(media_info.tracks[1].mastering_display_color_primaries)
            i = 0
            a = []
            b = []
            for track in media_info.tracks:
                if track.track_type == 'Audio' and i < 9:
                    i = i + 1
                    a.append(track.format)
                    b.append(round(track.sampling_rate/1000))
                    if i == 1:
                        self.label_audio_1.setText(a[0] + "  " + str(b[0]) + " KHz")
                    if i == 2:
                        self.label_audio_2.setText(a[1] + "  " + str(b[1]) + " KHz")
                    if i == 3:
                        self.label_audio_3.setText(a[2] + "  " + str(b[2]) + " KHz")
                    if i == 4:
                        self.label_audio_4.setText(a[3] + "  " + str(b[3]) + " KHz")
                    if i == 5:
                        self.label_audio_5.setText(a[4] + "  " + str(b[4]) + " KHz")
                    if i == 6:
                        self.label_audio_6.setText(a[5] + "  " + str(b[5]) + " KHz")
                    if i == 7:
                        self.label_audio_7.setText(a[6] + "  " + str(b[6]) + " KHz")
                    if i == 8:
                        self.label_audio_8.setText(a[7] + "  " + str(b[7]) + " KHz")
                    if i == 9:
                        self.label_audio_9.setText(a[8] + "  " + str(b[8]) + " KHz")
            self.comboBox_9.addItems([color_primaries.replace('.', '')])
            self.comboBox_10.addItems([matrix_coefficients.replace(' ', '').replace('.', '').replace('on-', '').replace('onstant', '')])
            self.comboBox_11.addItems([transfer_characteristics.replace('.', '')])
            if len(mastering_display_color_primaries) > 10:
                mdcp = mastering_display_color_primaries.split(',')
                r = mdcp[0].replace('R: ', '').replace('x=', '').replace(' ', ',').replace('y=', '').replace('000', '0').replace('00', '0')
                g = mdcp[1].replace(' G: ', '').replace('x=', '').replace(' ', ',').replace('y=', '').replace('000', '0').replace('00', '0')
                b = mdcp[2].replace(' B: ', '').replace('x=', '').replace(' ', ',').replace('y=', '').replace('000', '0').replace('00', '0')
                w = mdcp[3].replace(' White point: ', '').replace('x=', '').replace(' ', ',').replace('y=', '').replace('000', '0').replace('00', '0')
                chr_coord = r + "," + g + "," + b
                white_coord = w
                hdr10 = chr_coord
                hdr11 = white_coord
                self.comboBox_15.addItems(["Undefined", "Display P3", "DCI P3", "BT.2020", "BT.709"])
            else:
                self.comboBox_15.addItems([mastering_display_color_primaries, "Display P3", "DCI P3", "BT.2020", "BT.709"])
            self.comboBox_16.addItems([color_range, "Limited", "Full"])
            self.lineEdit_3.setText(maxCll.replace(' cd/m2', ''))
            self.lineEdit_4.setText(maxFall.replace(' cd/m2', ''))
            self.lineEdit_5.setText(max_luminance)
            self.lineEdit_6.setText(min_luminance)
            self.label_format.setText(fmt)
            self.label_resolution.setText(width + "x" + height)
            self.label_duration.setText(str(h) + "h " + str(m) + "m " + str(s) + "s")
            self.label_fps.setText(fps)
            self.label_subsampling.setText(chroma_subsampling)
            self.label_bitrate.setText(bit_rate + " Mbps")
            self.label_aspect_ratio.setText(aspect_ratio)
            self.label_color_space.setText(color_space)
            self.label_bit_depth.setText(bit_depth + " bit")
            hdr1 = self.comboBox_16.currentText()
            hdr2 = self.comboBox_9.currentText()
            hdr3 = self.comboBox_10.currentText()
            hdr4 = self.comboBox_11.currentText()
            hdr5 = self.lineEdit_5.text()
            hdr6 = self.lineEdit_6.text()
            hdr7 = self.lineEdit_3.text()
            hdr8 = self.lineEdit_4.text()
            hdr9 = self.comboBox_15.currentText()
        except:
            pass

    def save_file(self):
        self.lineEdit_2.clear()  # Clear elements
        prefix = self.comboBox_2.currentText()
        file_name_save = QtWidgets.QFileDialog.getSaveFileName(self, "Save As", "Untitled." + prefix.lower(), "All Files (*.*)")
        self.lineEdit_2.setText(file_name_save[0])  # Output filename to listEdit_2

    def task_complete(self):

        self.task_complete_app = TaskCompleteApp()
        self.task_complete_app.show()
        self.task_complete_app.textBrowser.clear()
        self.task_complete_app.textBrowser.append(message)
        self.task_complete_app.pushButton_4.clicked.connect(self.task_complete_app.close)

    def about(self):
        self.aboutapp = AboutApp()
        self.aboutapp.show()
        self.aboutapp.toolButton_6.clicked.connect(self.aboutapp.close)
        self.aboutapp.label_11.setText('<a href="https://github.com/CineEncoder/cine-encoder"> https://github.com/CineEncoder/cine-encoder </a>')
        self.aboutapp.label_11.setOpenExternalLinks(True)
        self.aboutapp.label_12.setText('<a href="https://github.com/CineEncoder/cine-encoder/blob/master/LICENSE"> License: GNU Lesser General Public License Version 3 </a>')
        self.aboutapp.label_12.setOpenExternalLinks(True)

    def donate(self):
        self.donateapp = DonateApp()
        self.donateapp.show()
        self.donateapp.toolButton_7.clicked.connect(lambda: webbrowser.open('https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU'))
        self.donateapp.toolButton_8.clicked.connect(lambda: webbrowser.open('https://blockchain.com/btc/payment_request?address=14ukiWCK2f8vFNPP3qKbo2vfaSGRMN63qL&amount=0.00005448&message=Cine Encoder donation'))

# -------------------------------------------- Settings menu ----------------------------------------------------------

    def settings_menu(self):
        self.comboBox_16.clear()
        self.comboBox_9.clear()
        self.comboBox_10.clear()
        self.comboBox_11.clear()
        self.lineEdit_5.clear()
        self.lineEdit_6.clear()
        self.lineEdit_3.clear()
        self.lineEdit_4.clear()
        self.comboBox_15.clear()


        self.comboBox_2.setEnabled(True)
        self.comboBox_3.setEnabled(True)
        self.comboBox_4.setEnabled(True)
        self.comboBox_12.setEnabled(True)
        self.comboBox_2.clear()
        self.comboBox_3.clear()
        self.comboBox_4.clear()
        self.comboBox_12.clear()
        try:
            if self.comboBox_1.currentText() == "H265  10 bit":
                self.comboBox_2.addItems(["MKV"])
                self.comboBox_3.addItems(["CPU", "NVENC"])
                self.comboBox_4.addItems(["VBR", "CQP"])
                self.comboBox_12.addItems(["From source", "AAC", "AC3"])
                self.comboBox_2.setEnabled(False)
                self.comboBox_16.addItems([hdr1, "Limited", "Full"])
                self.comboBox_9.addItems([hdr2])
                self.comboBox_10.addItems([hdr3])
                self.comboBox_11.addItems([hdr4])
                self.lineEdit_5.setText(hdr5)
                self.lineEdit_6.setText(hdr6)
                self.lineEdit_3.setText(hdr7)
                self.lineEdit_4.setText(hdr8)
                self.comboBox_15.addItems([hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"])

            if self.comboBox_1.currentText() == "H265  8 bit":
                self.comboBox_2.addItems(["MKV", "MOV", "MP4"])
                self.comboBox_3.addItems(["CPU", "NVENC"])
                self.comboBox_4.addItems(["VBR", "CQP"])
                self.comboBox_12.addItems(["From source", "AAC", "AC3"])
                self.comboBox_16.addItems(["Unsprt"])
                self.comboBox_9.addItems(["Unsprt"])
                self.comboBox_10.addItems(["Unsprt"])
                self.comboBox_11.addItems(["Unsprt"])
                self.lineEdit_5.setText("Unsprt")
                self.lineEdit_6.setText("Unsprt")
                self.lineEdit_3.setText("Unsprt")
                self.lineEdit_4.setText("Unsprt")
                self.comboBox_15.addItems(["Unsprt"])

            if self.comboBox_1.currentText() == "H264  8 bit":
                self.comboBox_2.addItems(["MKV", "MOV", "MP4"])
                self.comboBox_3.addItems(["CPU", "NVENC"])
                self.comboBox_4.addItems(["VBR", "CQP"])
                self.comboBox_12.addItems(["From source", "AAC", "AC3"])
                self.comboBox_16.addItems(["Unsprt"])
                self.comboBox_9.addItems(["Unsprt"])
                self.comboBox_10.addItems(["Unsprt"])
                self.comboBox_11.addItems(["Unsprt"])
                self.lineEdit_5.setText("Unsprt")
                self.lineEdit_6.setText("Unsprt")
                self.lineEdit_3.setText("Unsprt")
                self.lineEdit_4.setText("Unsprt")
                self.comboBox_15.addItems(["Unsprt"])

            if self.comboBox_1.currentText() == "VP9  10 bit":
                self.comboBox_2.addItems(["MKV"])
                self.comboBox_3.addItems(["CPU"])
                self.comboBox_4.addItems(["VBR", "CQP"])
                self.comboBox_12.addItems(["From source", "Vorbis", "Opus"])
                self.comboBox_2.setEnabled(False)
                self.comboBox_3.setEnabled(False)
                self.comboBox_16.addItems([hdr1, "Limited", "Full"])
                self.comboBox_9.addItems([hdr2])
                self.comboBox_10.addItems([hdr3])
                self.comboBox_11.addItems([hdr4])
                self.lineEdit_5.setText(hdr5)
                self.lineEdit_6.setText(hdr6)
                self.lineEdit_3.setText(hdr7)
                self.lineEdit_4.setText(hdr8)
                self.comboBox_15.addItems([hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"])

            if self.comboBox_1.currentText() == "AV1  10 bit":
                self.comboBox_2.addItems(["MKV"])
                self.comboBox_3.addItems(["CPU"])
                self.comboBox_4.addItems(["ABR", "CRF"])
                self.comboBox_12.addItems(["From source", "Vorbis", "Opus"])
                self.comboBox_2.setEnabled(False)
                self.comboBox_3.setEnabled(False)
                self.comboBox_16.addItems([hdr1, "Limited", "Full"])
                self.comboBox_9.addItems([hdr2])
                self.comboBox_10.addItems([hdr3])
                self.comboBox_11.addItems([hdr4])
                self.lineEdit_5.setText(hdr5)
                self.lineEdit_6.setText(hdr6)
                self.lineEdit_3.setText(hdr7)
                self.lineEdit_4.setText(hdr8)
                self.comboBox_15.addItems([hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"])

            if self.comboBox_1.currentText() == "ProRes HQ" or self.comboBox_1.currentText() == "DNxHR HQX":
                self.comboBox_2.addItems(["MKV"])
                self.comboBox_3.addItems(["CPU"])
                self.comboBox_4.addItems(["Auto"])
                self.comboBox_12.addItems(["PCM"])
                self.comboBox_2.setEnabled(False)
                self.comboBox_3.setEnabled(False)
                self.comboBox_4.setEnabled(False)
                self.comboBox_12.setEnabled(False)
                self.comboBox_16.addItems([hdr1, "Limited", "Full"])
                self.comboBox_9.addItems([hdr2])
                self.comboBox_10.addItems([hdr3])
                self.comboBox_11.addItems([hdr4])
                self.lineEdit_5.setText(hdr5)
                self.lineEdit_6.setText(hdr6)
                self.lineEdit_3.setText(hdr7)
                self.lineEdit_4.setText(hdr8)
                self.comboBox_15.addItems([hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"])
        except:
            pass

    def settings_menu_mode(self):
        self.comboBox_5.setEnabled(True)
        self.comboBox_6.setEnabled(True)
        self.comboBox_5.clear()
        self.comboBox_6.clear()
        if self.comboBox_4.currentText() == "Auto":
            self.label_credits_19.setText("Bitrate:")
            self.label_credits_20.show()
            self.comboBox_6.show()
            self.comboBox_5.addItems(["Auto"])
            self.comboBox_6.addItems(["Auto"])
            self.comboBox_5.setEnabled(False)
            self.comboBox_6.setEnabled(False)
        if self.comboBox_4.currentText() == "CQP":
            self.label_credits_19.setText("Quant:")
            self.label_credits_20.hide()
            self.comboBox_6.hide()
            self.comboBox_5.addItems(["19", "30", "29", "28", "27", "26", "25", "24", "23", "22", "21", "20", "18", "17", "16", "15"])
        if self.comboBox_4.currentText() == "VBR":
            self.label_credits_19.setText("Bitrate:")
            self.label_credits_20.show()
            self.comboBox_6.show()
            self.comboBox_5.addItems(["45M", "40M", "35M", "30M", "25M", "20M", "15M", "10M", "5M"])
            self.comboBox_6.addItems(["50M", "45M", "40M", "35M", "30M", "25M", "20M", "15M", "10M", "5M"])
        if self.comboBox_4.currentText() == "ABR":
            self.label_credits_19.setText("Bitrate:")
            self.label_credits_20.hide()
            self.comboBox_6.hide()
            self.comboBox_5.addItems(["45M", "40M", "35M", "30M", "25M", "20M", "15M", "10M", "5M"])
        if self.comboBox_4.currentText() == "CRF":
            self.label_credits_19.setText("Quality:")
            self.label_credits_20.hide()
            self.comboBox_6.hide()
            self.comboBox_5.addItems(["19", "30", "29", "28", "27", "26", "25", "24", "23", "22", "21", "20", "18", "17", "16", "15"])

    def settings_hdr(self):
        self.lineEdit_7.clear()
        self.lineEdit_8.clear()
        if self.comboBox_15.currentText() == "Unsprt":
            self.lineEdit_7.setText("Unsprt")
            self.lineEdit_8.setText("Unsprt")
        if self.comboBox_15.currentText() == "Undefined":
            self.lineEdit_7.setText(hdr10)
            self.lineEdit_8.setText(hdr11)
        if self.comboBox_15.currentText() == "Display P3":
            self.lineEdit_7.setText("0.680,0.320,0.265,0.690,0.150,0.060")
            self.lineEdit_8.setText("0.3127,0.3290")
        if self.comboBox_15.currentText() == "DCI P3":
            self.lineEdit_7.setText("0.680,0.320,0.265,0.690,0.150,0.060")
            self.lineEdit_8.setText("0.314,0.3510")
        if self.comboBox_15.currentText() == "BT.2020":
            self.lineEdit_7.setText("0.708,0.292,0.170,0.797,0.131,0.046")
            self.lineEdit_8.setText("0.3127,0.3290")
        if self.comboBox_15.currentText() == "BT.709":
            self.lineEdit_7.setText("0.640,0.330,0.30,0.60,0.150,0.060")
            self.lineEdit_8.setText("0.3127,0.3290")

    def settings_menu_audio(self):
        self.comboBox_13.setEnabled(True)
        self.comboBox_13.clear()
        if self.comboBox_12.currentText() == "From source":
            self.comboBox_13.addItems(["From source"])
            self.comboBox_13.setEnabled(False)
        if self.comboBox_12.currentText() == "AAC":
            self.comboBox_13.addItems(["384k", "256k", "128k"])
        if self.comboBox_12.currentText() == "AC3":
            self.comboBox_13.addItems(["640k", "448k", "384k", "256k"])
        if self.comboBox_12.currentText() == "Vorbis":
            self.comboBox_13.addItems(["448k", "384k", "256k", "128k", "96k", "64k"])
        if self.comboBox_12.currentText() == "Opus":
            self.comboBox_13.addItems(["448k", "384k", "256k", "128k", "96k", "64k"])
        if self.comboBox_12.currentText() == "PCM":
            self.comboBox_13.addItems(["Auto"])
            self.comboBox_13.setEnabled(False)

# ------------------------------------------------ Encode File ---------------------------------------------------------

    def make_preset(self):
        global preset_0, preset, preset_mkvmerge, loop_start
        loop_start = round(time.perf_counter())
        threads = int(multiprocessing.cpu_count())
        vbitrate = str(self.comboBox_5.currentText())
        maxrate = str(self.comboBox_6.currentText())
        quality = str(self.comboBox_5.currentText())
        colorprim_curr = str(self.comboBox_9.currentText()).lower()
        colormatrix_curr = str(self.comboBox_10.currentText()).lower()
        color_range_curr = str(self.comboBox_16.currentText())
        transfer_curr = str(self.comboBox_11.currentText())
        max_lum_curr = str(self.lineEdit_5.text())
        min_lum_curr = str(self.lineEdit_6.text())
        max_cll_curr = str(self.lineEdit_3.text())
        max_fall_curr = str(self.lineEdit_4.text())
        chroma_coord_curr = str(self.lineEdit_7.text())
        white_coord_curr = str(self.lineEdit_8.text())
        colorprim = ""
        colorprim_mkv = ""
        colormatrix = ""
        colormatrix_mkv = ""
        transfer = ""
        transfer_mkv = ""
        color_range = ""
        color_range_mkv = ""
        max_lum = ""
        min_lum = ""
        max_cll = ""
        max_fall = ""
        chroma_coord = ""
        white_coord = ""

# ----------------------------------------------------------------------------------------------------------------------
        if color_range_curr == "Limited":
            color_range = "-color_range tv "
            color_range_mkv = "--colour-range 0:1 "

        if color_range_curr == "Full":
            color_range = "-color_range pc "
            color_range_mkv = "--colour-range 0:2 "
# ----------------------------------------------------------------------------------------------------------------------
        if colorprim_curr == "bt709":
            colorprim = "-color_primaries bt709 "
            colorprim_mkv = "--colour-primaries 0:1 "

        if colorprim_curr == "bt2020":
            colorprim = "-color_primaries bt2020 "
            colorprim_mkv = "--colour-primaries 0:9 "
# ----------------------------------------------------------------------------------------------------------------------
        if colormatrix_curr == "bt709":
            colormatrix = "-colorspace bt709 "
            colormatrix_mkv = "--colour-matrix 0:1 "

        if colormatrix_curr == "bt2020nc":
            colormatrix = "-colorspace bt2020nc "
            colormatrix_mkv = "--colour-matrix 0:9 "

        if colormatrix_curr == "bt2020c":
            colormatrix = "-colorspace bt2020c "
            colormatrix_mkv = "--colour-matrix 0:10 "
# ----------------------------------------------------------------------------------------------------------------------
        if transfer_curr == "BT.709":
            transfer = "-color_trc bt709 "
            transfer_mkv = "--colour-transfer-characteristics 0:1 "

        if transfer_curr == "PQ":
            transfer = "-color_trc smpte2084 "
            transfer_mkv = "--colour-transfer-characteristics 0:16 "

        if transfer_curr == "HLG":
            transfer = "-color_trc arib-std-b67 "
            transfer_mkv = "--colour-transfer-characteristics 0:18 "

        if transfer_curr == "BT.2020 (10-bit)":
            transfer = "-color_trc bt2020-10 "
            transfer_mkv = "--colour-transfer-characteristics 0:14 "
# ----------------------------------------------------------------------------------------------------------------------
        if max_lum_curr != "None":
            max_lum = str(f'--max-luminance 0:{max_lum_curr} ')
        if max_lum_curr == "":
            max_lum = ""

        if min_lum_curr != "None":
            min_lum = str(f'--min-luminance 0:{min_lum_curr} ')
        if min_lum_curr == "":
            min_lum = ""

        if max_cll_curr != "None":
            max_cll = str(f'--max-content-light 0:{max_cll_curr} ')
        if max_cll_curr == "":
            max_cll = ""

        if max_fall_curr != "None":
            max_fall = str(f'--max-frame-light 0:{max_fall_curr} ')
        if max_fall_curr == "":
            max_fall = ""

        if chroma_coord_curr != "None":
            chroma_coord = str(f'--chromaticity-coordinates 0:{chroma_coord_curr} ')
        if chroma_coord_curr == "":
            chroma_coord = ""

        if white_coord_curr != "None":
            white_coord = str(f'--white-colour-coordinates 0:{white_coord_curr} ')
        if white_coord_curr == "":
            white_coord = ""

# ----------------------------------------------------------------------------------------------------------------------
        abitrate = str(self.comboBox_13.currentText())
        if self.comboBox_12.currentText() == "From source":
            apreset = "-c:a copy"
        if self.comboBox_12.currentText() == "AAC":
            apreset = f'-c:a aac -b:a {abitrate} '
        if self.comboBox_12.currentText() == "AC3":
            apreset = f'-c:a ac3 -b:a {abitrate} '
        if self.comboBox_12.currentText() == "Vorbis":
            apreset = f'-c:a libvorbis -b:a {abitrate} '
        if self.comboBox_12.currentText() == "Opus":
            apreset = f'-c:a libopus -b:a {abitrate} '
        if self.comboBox_12.currentText() == "PCM":
            apreset = "-c:a pcm_s16le"


        preset_mkvmerge = str(f'{colormatrix_mkv}{color_range_mkv}{transfer_mkv}{colorprim_mkv}{max_cll}{max_fall}{max_lum}{min_lum}{chroma_coord}{white_coord} ')


        if self.comboBox_1.currentText() == "H265  10 bit" and self.comboBox_3.currentText() == "NVENC": #-----H265 NVENC-10bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt p010le -c:v hevc_nvenc -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -profile:v main10 {colorprim}{transfer}{colormatrix}{color_range}-flags -global_header -max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt p010le -b:v {vbitrate} -maxrate:v {maxrate} -c:v hevc_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 -profile:v main10 {colorprim}{transfer}{colormatrix}{color_range}-flags -global_header -max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = str(f'-hide_banner -threads:v {threads/2} -threads:a {threads} -hwaccel cuda ')
            self.encode_mux_file()

        if self.comboBox_1.currentText() == "H265  10 bit" and self.comboBox_3.currentText() == "CPU": #-------H265-10bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt yuv420p10le -c:v libx265 -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -sei hdr -profile:v main10 {colorprim}{transfer}{colormatrix}{color_range}-max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt yuv420p10le -b:v {vbitrate} -maxrate:v {maxrate} -c:v libx265 -preset slow -rc vbr_hq -rc-lookahead 32 -sei hdr -profile:v main10 {colorprim}{transfer}{colormatrix}{color_range}-max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = "-hide_banner "
            self.encode_mux_file()

        if self.comboBox_1.currentText() == "VP9  10 bit": #-----------------------------------------------VP9-HDR-10bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -speed 4 -pix_fmt yuv420p10le {colorprim}{transfer}{colormatrix}{color_range}-rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -profile:v 2 -vcodec libvpx-vp9 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-b:v {vbitrate} -speed 4 -pix_fmt yuv420p10le {colorprim}{transfer}{colormatrix}{color_range}-maxrate {maxrate} -minrate 8040000 -profile:v 2 -vcodec libvpx-vp9 {apreset} -f matroska ')
            preset_0 = "-hide_banner "
            self.encode_mux_file()

        if self.comboBox_1.currentText() == "AV1  10 bit": #-----------------------------------------------AV1-HDR-10bit
            if self.comboBox_4.currentText() == "CRF":
                preset = str(f'-c:v libaom-av1 -crf {quality} -b:v 0 -strict -2 {colorprim}{transfer}{colormatrix}{color_range}{apreset} -f matroska ')
            if self.comboBox_4.currentText() == "ABR":
                preset = str(f'-c:v libaom-av1 -b:v {vbitrate} -strict -2 {colorprim}{transfer}{colormatrix}{color_range}{apreset} -f matroska ')
            preset_0 = "-hide_banner "
            self.encode_mux_file()

        if self.comboBox_1.currentText() == "H265  8 bit" and self.comboBox_3.currentText() == "NVENC": #---H265 NVENC-8bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt yuv420p -c:v hevc_nvenc -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -profile:v main -max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt yuv420p -b:v {vbitrate} -maxrate:v {maxrate} -c:v hevc_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 -profile:v main -max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = "-hide_banner -hwaccel cuda "
            self.encode_file()

        if self.comboBox_1.currentText() == "H265  8 bit" and self.comboBox_3.currentText() == "CPU": #--------H265-8bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt yuv420p -c:v libx265 -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -profile:v main -max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt yuv420p -b:v {vbitrate} -maxrate:v {maxrate} -c:v libx265 -preset slow -rc vbr_hq -rc-lookahead 32 -profile:v main -max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = "-hide_banner "
            self.encode_file()

        if self.comboBox_1.currentText() == "H264  8 bit" and self.comboBox_3.currentText() == "NVENC": #---H264 NVENC-8bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt yuv420p -c:v h264_nvenc -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -profile:v high -max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt yuv420p -b:v {vbitrate} -maxrate:v {maxrate} -c:v h264_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 -profile:v high -max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = "-hide_banner -hwaccel cuda "
            self.encode_file()

        if self.comboBox_1.currentText() == "H264  8 bit" and self.comboBox_3.currentText() == "CPU": #-------H264-8bit
            if self.comboBox_4.currentText() == "CQP":
                preset = str(f'-b:v 0 -pix_fmt yuv420p -c:v libx264 -preset slow -rc vbr_hq -cq {quality} -qmin {quality} -qmax {quality} -rc-lookahead 32 -profile:v high -max_muxing_queue_size 1024 {apreset} -f matroska ')
            if self.comboBox_4.currentText() == "VBR":
                preset = str(f'-pix_fmt yuv420p -b:v {vbitrate} -maxrate:v {maxrate} -c:v libx264 -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 -profile:v high -max_muxing_queue_size 1024 {apreset} -f matroska ')
            preset_0 = "-hide_banner "
            self.encode_file()

        if self.comboBox_1.currentText() == "ProRes HQ": #--------------------------------------------------ProRes-10bit
            preset = str(f'-c:v prores_ks -profile:v 3 -vtag apch {colorprim}{transfer}{colormatrix}{color_range}-c:a pcm_s16le ')
            preset_0 = "-hide_banner "
            self.encode_mux_file()

        if self.comboBox_1.currentText() == "DNxHR HQX": #---------------------------------------------------DNxHR-10bit
            preset = str(f'-c:v dnxhd -profile:v dnxhr_hqx -pix_fmt yuv422p10le {colorprim}{transfer}{colormatrix}{color_range}-c:a pcm_s16le ')
            preset_0 = "-hide_banner "
            self.encode_mux_file()

    def encode_mux_file(self):
        global fr_count, message
        global output_file
        global temp_folder
        global temp_file
        self.procedure_1 = QProcess()
        self.procedure_1.setProcessChannelMode(QProcess.MergedChannels)
        self.procedure_1.readyRead.connect(self.progress_1)
        self.procedure_1.finished.connect(self.mux)
        self.pushButton_3.setEnabled(False)
        input_file = self.lineEdit_1.text()
        output_file = self.lineEdit_2.text()
        temp_folder = output_file + "_temp"
        temp_file = temp_folder + "/temp.mkv"
        try:
            os.mkdir(temp_folder)
        except:
            self.pushButton_3.setEnabled(True)
            message = "Please delete previous temporary folder or rename output file."
            self.task_complete()
        else:
            try:
                media_info = MediaInfo.parse(input_file)
                dur = 0.001*float(media_info.tracks[0].duration)
                fps = float(media_info.tracks[0].frame_rate)
                dur_mod = round(dur)
                fps_mod = round(fps, 2)
                fr_count = round(dur_mod*fps_mod)
            except:
                self.pushButton_3.setEnabled(True)
                message = "Select first input and output file!"
                self.task_complete()
            else:
                self.label_53.show()
                self.label_53.setText("Encoding:")
                self.label_54.show()
                self.label_55.show()
                self.progressBar.show()
                self.progressBar.setProperty("value", 0)
                try:
                    cmd = f'ffmpeg {preset_0}-i "{input_file}" {preset} -y "{temp_file}" '
                    self.procedure_1.start(cmd)
                except:
                    self.pushButton_3.setEnabled(True)
                    message = "An error occured!\nPossible reasons for the error:\n - input or output file not defined,\n - codec may not be supported by program,\n - incorrect encoder settings."
                    self.task_complete()


    def mux(self):
        global message
        self.procedure_1.deleteLater()
        self.procedure_2 = QProcess()
        self.procedure_2.setProcessChannelMode(QProcess.MergedChannels)
        self.procedure_2.readyRead.connect(self.progress_2)
        self.procedure_2.finished.connect(self.complete_mux)
        self.label_53.setText("Muxing:")
        self.progressBar.setProperty("value", 0)
        try:
            cmd = f'mkvmerge -o "{output_file}" {preset_mkvmerge} "{temp_file}" '
            self.procedure_2.start(cmd)
        except:
            self.pushButton_3.setEnabled(True)
            message = "An error occured!\nPossible reasons for the error:\n - input or output file not defined,\n - codec may not be supported by program,\n - incorrect encoder settings."
            self.task_complete()

    def complete_mux(self):
        global message
        self.procedure_2.deleteLater()
        self.pushButton_3.setEnabled(True)
        message = "Task completed.\nCheck whether the resulting file matches the specified parameters.\nPlease delete temporary folder manually."
        self.task_complete()
        #try:
        #    os.replace(temp_file)
        #    os.rmdir(temp_folder)
        #except:
        #    pass

    def encode_file(self):
        global fr_count, message
        self.procedure_1 = QProcess()
        self.procedure_1.setProcessChannelMode(QProcess.MergedChannels)
        self.procedure_1.readyRead.connect(self.progress_1)
        self.procedure_1.finished.connect(self.complete)
        self.pushButton_3.setEnabled(False)
        input_file = self.lineEdit_1.text()
        output_file = self.lineEdit_2.text()
        try:
            media_info = MediaInfo.parse(input_file)
            dur = 0.001*float(media_info.tracks[0].duration)
            fps = float(media_info.tracks[0].frame_rate)
            dur_mod = round(dur)
            fps_mod = round(fps, 2)
            fr_count = round(dur_mod*fps_mod)
        except:
            self.pushButton_3.setEnabled(True)
            message = "Select first input and output file!"
            self.task_complete()
        else:
            self.label_53.show()
            self.label_53.setText("Encoding:")
            self.label_54.show()
            self.label_55.show()
            self.progressBar.show()
            self.progressBar.setProperty("value", 0)
            try:
                cmd = f'ffmpeg {preset_0}-i "{input_file}" {preset} -y "{output_file}" '
                self.procedure_1.start(cmd)
            except:
                self.pushButton_3.setEnabled(True)
                message = "An error occured!\nPossible reasons for the error:\n - input or output file not defined,\n - codec may not be supported by program,\n - incorrect encoder settings."
                self.task_complete()

    def complete(self):
        global message
        self.procedure_1.deleteLater()
        self.pushButton_3.setEnabled(True)
        message = "Task completed.\nCheck whether the resulting file matches the specified parameters."
        self.task_complete()

    def progress_1(self):
        line = str(self.procedure_1.readAllStandardOutput())
        line_mod6 = line.replace('   ', ' ').replace('  ', ' ').replace('  ', ' ').replace('= ', '=').replace("b'", "").replace("r'", "").replace("\\", "")
        pos_st = line_mod6.find('frame=') + 6
        if pos_st != 5:
            data = line_mod6.split(' ')
            frame = int(data[0].replace('frame=', ''))
            iter_start = round(time.perf_counter())
            timer = iter_start - loop_start
            full_time = round((timer*fr_count)/(frame+1))
            rem_time = full_time - timer
            h = math.trunc(rem_time / 3600)
            m = math.trunc((rem_time - (h * 3600)) / 60)
            s = math.trunc(rem_time - (h * 3600) - (m * 60))
            percent = (frame*100)/fr_count
            self.progressBar.setProperty("value", percent)
            self.label_55.setText(str(h).rjust(2, '0') + ":" + str(m).rjust(2, '0') + ":" + str(s).rjust(2, '0'))

    def progress_2(self):
        line = str(self.procedure_2.readAllStandardOutput())
        line_mod5 = line.replace('   ', ' ').replace('.', '').replace("b'", "").replace("n'", "").replace('\\', '').replace("r'", "")
        pos_st = line_mod5.find('Progress: ') + 10
        pos_end = line_mod5.find('%')
        if pos_st != 9:
            percent = int(line_mod5[pos_st:pos_end])
            self.progressBar.setProperty("value", percent)


    def stop(self):
        global message
        try:
            self.procedure_1.kill()
        except:
            pass
        try:
            self.procedure_2.kill()
        except:
            pass
        self.pushButton_3.setEnabled(True)
        message = "All processes are stopped."
        self.task_complete()

# --------------------------------------------------------------------------------------------------------------------

class TaskCompleteApp(QtWidgets.QMainWindow, ui_taskcomplete.Ui_TaskComplete):
    def __init__(self):  # init
        super().__init__()
        self.setupUi(self)  # init

class DonateApp(QtWidgets.QMainWindow, ui_donate.Ui_Donate):
    def __init__(self):  # init
        super().__init__()
        self.setupUi(self)  # init

class AboutApp(QtWidgets.QMainWindow, ui_about.Ui_About):
    def __init__(self):  # init
        super().__init__()
        self.setupUi(self)  # init


def main():
    app = QtWidgets.QApplication(sys.argv)
    window = ExampleApp()
    window.show()
    app.exec_()

if __name__ == '__main__':  # if run direct
    main()  # run main()
