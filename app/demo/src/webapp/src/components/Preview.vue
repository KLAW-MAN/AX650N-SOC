<template>
  <div class="total_container">
    <el-container class="preview_outside_container" >
      <el-container :class="mode_option == 'ai' ? ( autoSnapshotEnable && flashEnable ? 'preview_main_container_ai_2cap' : !autoSnapshotEnable && !flashEnable ? 'preview_main_container_ai' : 'preview_main_container_ai_1cap') : (autoSnapshotEnable && flashEnable ? 'preview_main_container_normal_2cap' : !autoSnapshotEnable && !flashEnable ? 'preview_main_container_normal' : 'preview_main_container_normal_1cap')">
        <el-main class="preview_capture_left_container">
          <el-container id="video_container_0" class="preview_container">
            <el-main ref="frtMain0" class="video_container">
              <video id="myVideo0" v-show="streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].showVideoStatus" muted="isMuted" ref="frtVideoRef0" class="frtVideo" autoplay
                playsinline oncontextmenu="return false;">
              </video>
              <canvas id="myMjpeg0" v-show="!streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].showVideoStatus" class="frtVideo2">
              </canvas>
            </el-main>
            <el-footer class="video_controls_container" height="30px">
              <div class="left_controls_container">
                <div class="size_option">
                  <el-dropdown @command="changeSizeOption0" size="mini" placement="top-start" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text">{{ sizeOptions[streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].sizeOptionInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :append-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for="item in sizeOptions"
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div>
                  <el-dropdown @command="changeStream0" size="mini" placement="top" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text">{{
                      streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].videoInfo[streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].streamInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for="item in streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].videoInfo"
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div class="src_option" v-show="sensor_select === 1">
                  <el-dropdown @command="changeSensorOptLeft" size="mini" placement="top" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text">{{
                      sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for="item in sensorOptLeft.sensorInfo"
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div class="size_info">
                  <span>{{ this.streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].encInfo }}</span>
                </div>
              </div>
              <div class="right_controls_container">
                <div class="ezoom" >
                  <el-slider class="ezoom_slider" v-model="ezoom[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].ezoom_ratio" :min="0" :max="32" @mousedown.native="onEZoomMousedown" @input="onEZoom(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)" v-show="ezoomEnable && pano_sns_id !== 0"></el-slider>
                </div>
                <div class="draw_option" >
                  <el-tooltip effect="dark" :content="isEnable3ASR ? 'Disable 3A Sync Ratio' : 'Enable 3A Sync Ratio'" placement="top-end" :enterable="false">
                    <el-button
                      :class="{'draw_button_enable':isEnable3ASR, 'draw_button_disable': !isEnable3ASR}"
                      icon="el-icon-refresh" @click="change3ASR"
                      v-show="pano_sns_id === 0"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="!isTalking ? 'Start Talk' : 'Stop Talk'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini"
                      :class="{ 'draw_button_enable': !isTalking, 'draw_button_disable': isTalking }"
                      :icon="isTalking?'iconfont icon-mic-off':'iconfont icon-mic-on'" @click="onStartTalk()"
                      v-show="aenc_enable"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="!isMuted ? 'Muted' : 'Play sound'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini"
                      :class="{ 'draw_button_enable': isMuted, 'draw_button_disable': !isMuted }"
                      :icon="isMuted?'iconfont icon-silence':'iconfont icon-sound'" @click="onMute()"
                      v-show="aenc_enable"></el-button>
                  </el-tooltip>
                </div>
                <div class="trigger_flash">
                  <el-tooltip effect="dark" :content="'Flash'" placement="top-end">
                    <el-button size="mini" :class="{ 'draw_button_enable': true, 'draw_button_disable': false }"
                     icon ="el-icon-thumb" @click="doTriggerFlash(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)" v-show="streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].trigger_attr.trigger_enable"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="'Capture'" placement="top-end">
                    <el-button size="mini" :class="{ 'draw_button_enable': true, 'draw_button_disable': false }"
                      icon="el-icon-camera-solid" @click="onSnapshot(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)"
                      v-show="manualSnapshotEnable"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="'Input Facial Data'" placement="top-end" :enterable="false">
                    <el-button size="mini" :class="{ 'draw_button_enable': true, 'draw_button_disable': false }"
                      icon="el-icon-user-solid" @click="recordFaceinfo(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)"
                      v-show="isSearchImg && detect_mode == 'facehuman'"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="!streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].isRecStart ? 'Start Record' : 'Stop Record'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini" :disabled="!streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].recEnable"
                      class="draw_button_enable" :icon="streamOptions[sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id].recIcon"
                      @click="onRecording(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)"></el-button>
                  </el-tooltip>
                </div>
                <div class="full_option">
                  <el-tooltip effect="dark" :content="isFullScreen ? 'Exit Full Screen' : 'Enter Full Screen'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini" class="full_button" :icon="screenIcon" @click="doScreen(sensorOptLeft.sensorInfo[sensorOptLeft.snsOptInd].id)"></el-button>
                  </el-tooltip>
                </div>
              </div>
            </el-footer>
          </el-container>
        </el-main>
        <div class="preview_seperator_line" v-show="home_dual_mode === 1"></div>
        <el-main class="preview_capture_left_container" v-show="home_dual_mode === 1">
          <el-container id="video_container_1" class="preview_container">
            <el-main ref="frtMain1" class="video_container">
              <video id="myVideo1" v-show="streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].showVideoStatus" ref="frtVideoRef1" class="frtVideo" autoplay
                playsinline oncontextmenu="return false;">
              </video>
              <canvas id="myMjpeg1" v-show="!streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].showVideoStatus" class="frtVideo2">
              </canvas>
            </el-main>
            <el-footer class="video_controls_container" height="30px">
              <div class="left_controls_container">
                <div class="size_option">
                  <el-dropdown @command="changeSizeOption1" size="mini" placement="top-start" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text" :disabled="home_dual_mode != 1">{{
                      sizeOptions[streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].sizeOptionInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :append-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for=" item  in  sizeOptions "
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div>
                  <el-dropdown @command="changeStream1" size="mini" placement="top" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text" :disabled="home_dual_mode != 1">{{
                      streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].videoInfo[streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].streamInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :append-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for=" item  in  streamOptions[1].videoInfo "
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div class="src_option" v-show="sensor_select === 1">
                  <el-dropdown @command="changeSensorOptRight" size="mini" placement="top" trigger="click">
                    <el-button size="mini" class="el-dropdown-link" type="text">{{
                      sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].label
                    }}</el-button>
                    <el-dropdown-menu slot="dropdown" :-to-body="false">
                      <el-dropdown-item :command="item.id + ''" :icon="item.icon" v-for="item in sensorOptRight.sensorInfo"
                        :key="item.id">{{ item.label }}</el-dropdown-item>
                    </el-dropdown-menu>
                  </el-dropdown>
                </div>
                <div class="size_info">
                  <span calss="dynamic_font">{{ this.streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].encInfo }}</span>
                </div>
              </div>
              <div class="right_controls_container">
                <div class="ezoom" >
                  <el-slider class="ezoom_slider" v-model="ezoom[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].ezoom_ratio" :min="0" :max="32" @mousedown.native="onEZoomMousedown" @input="onEZoom(sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id)" v-show="ezoomEnable && pano_sns_id !== 1"></el-slider>
                </div>
                <div class="draw_option" >
                  <el-tooltip effect="dark" :content="isEnable3ASR ? 'Disable 3A Sync Ratio' : 'Enable 3A Sync Ratio'" placement="top-end" :enterable="false">
                    <el-button
                      :class="{'draw_button_enable':isEnable3ASR, 'draw_button_disable': !isEnable3ASR}"
                      icon="el-icon-refresh" @click="change3ASR"
                      v-show="pano_sns_id === 1"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="'Capture'" placement="top-end">
                    <el-button size="mini" :class="{ 'draw_button_enable': true, 'draw_button_disable': false }"
                      icon="el-icon-camera-solid" @click="onSnapshot(sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id)"
                      v-show="manualSnapshotEnable"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="'Input Facial Data'" placement="top-end" :enterable="false">
                    <el-button size="mini" :class="{ 'draw_button_enable': true, 'draw_button_disable': false }"
                      icon="el-icon-user-solid" @click="recordFaceinfo(sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id)"
                      v-show="isSearchImg && detect_mode == 'facehuman'"></el-button>
                  </el-tooltip>
                </div>
                <div class="draw_option">
                  <el-tooltip effect="dark" :content="!streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].isRecStart ? 'Start Record' : 'Stop Record'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini" :disabled="!streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].recEnable"
                      class="draw_button_enable" :icon="streamOptions[sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id].recIcon"
                      @click="onRecording(sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id)"></el-button>
                  </el-tooltip>
                </div>
                <div class="full_option">
                  <el-tooltip effect="dark" :content="isFullScreen ? 'Exit Full Screen' : 'Enter Full Screen'" placement="top-end"
                    :enterable="false">
                    <el-button size="mini" class="full_button" :icon="screenIcon" @click="doScreen(sensorOptRight.sensorInfo[sensorOptRight.snsOptInd].id)"
                      :disabled="home_dual_mode != 1"></el-button>
                  </el-tooltip>
                </div>
              </div>
            </el-footer>
          </el-container>
        </el-main>
        <el-aside class="side_identify_events_container" width="335px" v-show="mode_option == 'ai'">
          <el-container class="identify_events_container">
            <el-header class="identify_container">
              <el-row class="image_info_container">
                <el-col :span="10" class="image_col">
                  <!--
                  <el-image class="image-large" :src="currentImg.src" :preview-src-list="[currentImg.src]" fit="fill">
                    <div slot="error" class="image-slot-large">
                      <i class="el-icon-picture-outline"></i>
                    </div>
                  </el-image>
                  <el-image-viewer v-if="showViewer && currentImg.src != ''" :on-close="closeViewer" :url-list="[currentImg.src]"/>
                  -->
                  <div class="image-large">
                    <i class="el-icon-picture-outline" v-if="currentImg.src == ''"></i>
                    <img class="image-large" v-if="currentImg.src != ''" :src="currentImg.src" crossOrigin="anonymous"
                      @click="look(currentImg)"></img>
                  </div>
                </el-col>
                <el-col :span="14" class="info_col">
                  <div class="info_container">
                    <div class="info_text" v-show="detect_mode == 'facehuman'">
                      Gender: {{ currentImg.info.gender }}
                    </div>
                    <div class="info_text" v-show="detect_mode == 'facehuman'">
                      Age: {{ currentImg.info.age }}
                    </div>
                    <div class="info_text" v-show="detect_mode == 'facehuman'">
                      Mask: {{ currentImg.info.mask }}
                    </div>
                    <div class="info_text" v-show="detect_mode == 'facehuman'">
                      Recognition: {{ currentImg.info.info }}
                    </div>
                    <div class="info_text" v-show="detect_mode == 'hvcfp'">
                      Plate NO.: {{ currentImg.info.number }}
                    </div>
                    <div class="info_text" v-show="detect_mode == 'hvcfp'">
                      Plate Color: {{ currentImg.info.color }}
                    </div>
                  </div>
                </el-col>
              </el-row>
            </el-header>
            <el-main class="events_container">
              <el-table
                :header-cell-style="{ background: '#2C2C2C', color: '#B0B0B0', border: 'none', fontSize: '10px', padding: '4px' }"
                :cell-style="{ padding: '4px' }" :row-style="{ height: '16px' }" style="font-size: 10px" height="150"
                class="events_table" :data="eventList" highlight-current-row>
                <el-table-column type="index" label="#" width="50"></el-table-column>
                <el-table-column property="date" label="Time" width="105"> </el-table-column>
                <el-table-column property="sensor" label="Camera" width="85"> </el-table-column>
                <el-table-column property="type" label="Type" width="85"> </el-table-column>
              </el-table>
            </el-main>
          </el-container>
        </el-aside>
      </el-container>
      <el-footer :class="((autoSnapshotEnable && !flashEnable) || (!autoSnapshotEnable && flashEnable)) ? 'snapshot_img_container_half' : 'snapshot_img_container'" v-show="autoSnapshotEnable || flashEnable">
        <el-row class="img_list_top" v-show="autoSnapshotEnable">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Capture</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" snap_ind  in  image_count / 2 " :key="snap_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushSnapshots.img[snap_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushSnapshots.img[snap_ind - 1].src != ''"
                  :src="pushSnapshots.img[snap_ind - 1].src" @click="look(pushSnapshots.img[snap_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" snap_ind  in  image_count / 2 " :key="snap_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushSnapshots.img[image_count / 2 + snap_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushSnapshots.img[image_count / 2 + snap_ind - 1].src != ''"
                  :src="pushSnapshots.img[image_count / 2 + snap_ind - 1].src"
                  @click="look(pushSnapshots.img[image_count / 2 + snap_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row :class="autoSnapshotEnable ? 'img_list':'img_list_top'" v-show="flashEnable">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Flash</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" flash_ind  in  image_count / 2 " :key="flash_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushSnapshots.flash[flash_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushSnapshots.flash[flash_ind - 1].src != ''"
                  :src="pushSnapshots.flash[flash_ind - 1].src" @click="look(pushSnapshots.flash[flash_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" flash_ind  in  image_count / 2 " :key="flash_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushSnapshots.flash[image_count / 2 + flash_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushSnapshots.flash[image_count / 2 + flash_ind - 1].src != ''"
                  :src="pushSnapshots.flash[image_count / 2 + flash_ind - 1].src"
                  @click="look(pushSnapshots.flash[image_count / 2 + flash_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
      </el-footer>
      <el-footer class="hvcfp_img_container" v-show="detect_mode == 'hvcfp' && mode_option == 'ai'">
        <el-row class="img_list_top">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Face</div>
          </el-col>
          <el-col :span="11" class="hvcfp_img_col">
            <el-col class="hvcfp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushHCVFPImages.face[face_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.face[face_ind - 1].src != ''"
                  :src="pushHCVFPImages.face[face_ind - 1].src" @click="look(pushHCVFPImages.face[face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col :span="11" class="hvcfp_img_col">
            <el-col class="hvcfp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline"
                  v-if="pushHCVFPImages.face[image_count / 2 + face_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.face[image_count / 2 + face_ind - 1].src != ''"
                  :src="pushHCVFPImages.face[image_count / 2 + face_ind - 1].src"
                  @click="look(pushHCVFPImages.face[image_count / 2 + face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Body</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushHCVFPImages.body[body_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.body[body_ind - 1].src != ''"
                  :src="pushHCVFPImages.body[body_ind - 1].src" @click="look(pushHCVFPImages.body[body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline"
                  v-if="pushHCVFPImages.body[image_count / 2 + body_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.body[image_count / 2 + body_ind - 1].src != ''"
                  :src="pushHCVFPImages.body[image_count / 2 + body_ind - 1].src"
                  @click="look(pushHCVFPImages.body[image_count / 2 + body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Bicycle</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" cycle_ind  in  image_count / 2 " :key="cycle_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushHCVFPImages.cycle[cycle_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.cycle[cycle_ind - 1].src != ''"
                  :src="pushHCVFPImages.cycle[cycle_ind - 1].src"
                  @click="look(pushHCVFPImages.cycle[cycle_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" cycle_ind  in  image_count / 2 " :key="cycle_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline"
                  v-if="pushHCVFPImages.cycle[image_count / 2 + cycle_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.cycle[image_count / 2 + cycle_ind - 1].src != ''"
                  :src="pushHCVFPImages.cycle[image_count / 2 + cycle_ind - 1].src"
                  @click="look(pushHCVFPImages.cycle[image_count / 2 + cycle_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Vehicle</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" vechicle_ind  in  image_count / 2 " :key="vechicle_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushHCVFPImages.vechicle[vechicle_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.vechicle[vechicle_ind - 1].src != ''"
                  :src="pushHCVFPImages.vechicle[vechicle_ind - 1].src"
                  @click="look(pushHCVFPImages.vechicle[vechicle_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" vechicle_ind  in  image_count / 2 " :key="vechicle_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline"
                  v-if="pushHCVFPImages.vechicle[image_count / 2 + vechicle_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.vechicle[image_count / 2 + vechicle_ind - 1].src != ''"
                  :src="pushHCVFPImages.vechicle[image_count / 2 + vechicle_ind - 1].src"
                  @click="look(pushHCVFPImages.vechicle[image_count / 2 + vechicle_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list_bottom">
          <el-col class="hvcfp_img_col" :span="2">
            <div class="type_text">Plate</div>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" plate_ind  in  image_count / 2 " :key="plate_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline" v-if="pushHCVFPImages.plate[plate_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.plate[plate_ind - 1].src != ''"
                  :src="pushHCVFPImages.plate[plate_ind - 1].src"
                  @click="look(pushHCVFPImages.plate[plate_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="hvcfp_img_col" :span="11">
            <el-col class="hvcfp_img_col" :span="3" v-for=" plate_ind  in  image_count / 2 " :key="plate_ind">
              <div class="image-small">
                <i class="el-icon-picture-outline"
                  v-if="pushHCVFPImages.plate[image_count / 2 + plate_ind - 1].src == ''"></i>
                <img class="image-small" v-if="pushHCVFPImages.plate[image_count / 2 + plate_ind - 1].src != ''"
                  :src="pushHCVFPImages.plate[image_count / 2 + plate_ind - 1].src"
                  @click="look(pushHCVFPImages.plate[image_count / 2 + plate_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
      </el-footer>
      <el-footer class="fh_img_container" v-show="detect_mode == 'facehuman' && mode_option == 'ai'">
        <el-row class="img_list_top">
          <el-col class="fp_img_col" :span="2">
            <div class="type_text_fh">Face</div>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.face[face_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.face[face_ind - 1].src != ''"
                  :src="pushFHImages.face[face_ind - 1].src" @click="look(pushFHImages.face[face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.face[image_count / 2 + face_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.face[image_count / 2 + face_ind - 1].src != ''"
                  :src="pushFHImages.face[image_count / 2 + face_ind - 1].src"
                  @click="look(pushFHImages.face[image_count / 2 + face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list">
          <el-col class="fp_img_col" :span="2">
            <div class="type_text_fh">Face</div>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.face[image_count + face_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.face[image_count + face_ind - 1].src != ''"
                  :src="pushFHImages.face[image_count + face_ind - 1].src"
                  @click="look(pushFHImages.face[image_count + face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" face_ind  in  image_count / 2 " :key="face_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline"
                  v-if="pushFHImages.face[image_count / 2 + image_count + face_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.face[image_count / 2 + image_count + face_ind - 1].src != ''"
                  :src="pushFHImages.face[image_count / 2 + image_count + face_ind - 1].src"
                  @click="look(pushFHImages.face[image_count / 2 + image_count + face_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list">
          <el-col class="fp_img_col" :span="2">
            <div class="type_text_fh">Body</div>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.body[body_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.body[body_ind - 1].src != ''"
                  :src="pushFHImages.body[body_ind - 1].src" @click="look(pushFHImages.body[body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.body[image_count / 2 + body_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.body[image_count / 2 + body_ind - 1].src != ''"
                  :src="pushFHImages.body[image_count / 2 + body_ind - 1].src"
                  @click="look(pushFHImages.body[image_count / 2 + body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
        <el-row class="img_list_bottom">
          <el-col class="fp_img_col" :span="2">
            <div class="type_text_fh">Body</div>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline" v-if="pushFHImages.body[image_count + body_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.body[image_count + body_ind - 1].src != ''"
                  :src="pushFHImages.body[image_count + body_ind - 1].src"
                  @click="look(pushFHImages.body[image_count + body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
          <el-col class="fp_img_col" :span="11">
            <el-col class="fp_img_col" :span="3" v-for=" body_ind  in  image_count / 2 " :key="body_ind">
              <div class="image-medium">
                <i class="el-icon-picture-outline"
                  v-if="pushFHImages.body[image_count / 2 + image_count + body_ind - 1].src == ''"></i>
                <img class="image-medium" v-if="pushFHImages.body[image_count / 2 + image_count + body_ind - 1].src != ''"
                  :src="pushFHImages.body[image_count / 2 + image_count + body_ind - 1].src"
                  @click="look(pushFHImages.body[image_count / 2 + image_count + body_ind - 1])"></img>
              </div>
            </el-col>
          </el-col>
        </el-row>
      </el-footer>
      <div class="view_container" v-show="showViewer">
        <el-dialog :visible.sync="showViewer" width="800px" :before-close="handleViewerClose">
          <span slot="title" class="dialog-title">
            {{ image_types[viewerImg.type] + 'details-' + viewerImg.date }}
          </span>
          <el-row class="view_image_info_container">
            <el-col :span="12" class="view_image_col">
              <el-image class="image-view" :src="viewerImg.src" :preview-src-list="[viewerImg.src]" fit="fill">
                <div slot="error" class="image-slot-large">
                  <i class="el-icon-picture-outline"></i>
                </div>
              </el-image>
            </el-col>
            <el-col :span="12" class="info_col">
              <div class="view_info_container">
                <div class="view_info_text">
                  Video Source: {{ viewerImg.snsId }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 6">
                  Flash Frame Private Info:
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 6">
                  Timestamp:{{ viewerImg.info.pts }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 6">
                  Sequence NO.: {{ viewerImg.info.seqnum }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 6">
                  Flash ID: {{ viewerImg.info.userdata }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 3 && detect_mode == 'facehuman'">
                  Gender: {{ viewerImg.info.gender }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 3 && detect_mode == 'facehuman'">
                  Age: {{ viewerImg.info.age }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 3 && detect_mode == 'facehuman'">
                  Mask: {{ viewerImg.info.mask }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 3 && detect_mode == 'facehuman'">
                  Recognition: {{ viewerImg.info.info }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 4 && detect_mode == 'hvcfp'">
                  Plate NO.: {{ viewerImg.info.number }}
                </div>
                <div class="view_info_text" v-show="viewerImg.type == 4 && detect_mode == 'hvcfp'">
                  Plate Color: {{ viewerImg.info.color }}
                </div>
                <div class="view_info_text"
                  v-show="detect_mode == 'facehuman' && viewerImg.type != 3 || detect_mode == 'hvcfp' && viewerImg.type != 4 && viewerImg.type != 6">
                  No Attribute
                </div>
              </div>
            </el-col>
          </el-row>
          <!--
          <span slot="footer" class="dialog-footer">
            <el-button type="primary" @click="showViewer = false">OK</el-button>
          </span>
          -->
        </el-dialog>
      </div>
    </el-container>
  </div>
</template>

<script>
import Recorderx, { ENCODE_TYPE, RECORDER_STATE, audioTools } from "recorderx"
import Wfs from '../plugins/wfs-min.js'
import screenfull from 'screenfull'
import { saveAs } from "file-saver"

export default {
  props: ["mode_option", "home_dual_mode", "pano_sns_id", "sensor_select"],
  data() {
    return {
      image_count: 16,
      detect_mode: 'hvcfp',
      //detect_mode: 'facehuman',
      pushFHImages: {
        face: [],
        body: []
      },
      pushSnapshots: {
        img: [],
        flash: []
      },
      pushHCVFPImages: {
        face: [],
        body: [],
        vechicle: [],
        cycle: [],
        plate: []
      },
      currentImg: {
        snsId: 0,
        type: 0,
        src: '',
        date: '',
        info: {
          age: '--',
          gender: '--',
          mask: '--',
          info: '--',
          number: '--',
          color: '--'
        }
      },
      viewerImg: {
        snsId: 0,
        type: 0,
        src: '',
        date: '',
        info: {
          age: '--',
          gender: '--',
          mask: '--',
          info: '--',
          number: '--',
          color: '--'
        }
      },
      eventList: [
      ],
      image_types: ['Body', 'Vehicle', 'Bicycle', 'Face', 'Plate', 'Capture', 'Flash'],
      genderOptions: ['F', 'M'],
      respiratorOptions: {
        'no_respirator': 'No Respirator',
        'surgical': 'Surgical',
        'anti-pollution': 'Anti-Pollution',
        'common': 'Common',
        'kitchen_transparent': 'Kitchen Transparent',
        'unknown': 'Unknown'
      },
      plateColorOptions: {
        'blue': 'blue',
        'yellow': 'yellow',
        'black': 'black',
        'white': 'white',
        'green': 'green',
        'small_new_energy': 'new energy',
        'large_new_energy': 'new energy',
        'absence': 'Unknown',
        'unknown': 'Unknown'
      },
      sizeOptions: [
        {
          id: 0,
          label: 'Auto',
          value: '0:0',
          icon: 'el-icon-full-screen'
        },
        {
          id: 1,
          label: 'Original',
          value: '1:1',
          icon: 'el-icon-full-screen'
        }
      ],
      sns_num: 1, // 1-single, 2-dual, 4-quad
      streamOptions: [
        {
          streamInd: 0,
          sizeOptionInd: 1,
          recEnable: false,
          isRecStart: false,
          isWfsError: false,
          showVideoStatus: false,
          wfsObj: null,
          encInfo: '',
          timerEncInfo: undefined,
          recIcon: 'iconfont icon-record-start',
          videoInfo: [
            {
              id: 0,
              label: 'Main Stream0',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            },
            {
              id: 1,
              label: 'Sub Stream1',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            },
            {
              id: 2,
              label: 'Sub Stream2',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            }
          ],
          mjpegCtx: {
            isDrawImg: true,
            mjpegW: 0,
            mjpegH: 0,
            sizeOptionInd: 0,
            getObjectFitSize: this.getObjectFitSize,
            timerDraw: undefined
          },
          trigger_attr: {
            trigger_enable: false,
            trigger: false
          }
        },
        {
          streamInd: 0,
          sizeOptionInd: 1,
          recEnable: false,
          isRecStart: false,
          isWfsError: false,
          showVideoStatus: false,
          wfsObj: null,
          encInfo: '',
          timerEncInfo: undefined,
          recIcon: 'iconfont icon-record-start',
          videoInfo: [
            {
              id: 0,
              label: 'Main Stream0',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            },
            {
              id: 1,
              label: 'Sub Stream1',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            },
            {
              id: 2,
              label: 'Sub Stream2',
              icon: 'el-icon-full-screen',
              mediaType: 'H264Raw',
              mediaFPS: 25
            }
          ],
          mjpegCtx: {
            isDrawImg: true,
            mjpegW: 0,
            mjpegH: 0,
            sizeOptionInd: 0,
            getObjectFitSize: this.getObjectFitSize,
            timerDraw: undefined
          },
          trigger_attr: {
            trigger_enable: false,
            trigger: false
          }
        }
      ],
      sensorOptLeft: {
        snsOptInd: 0,
        sensorInfo: [
          {
            id: 0,
            label: 'CAM0',
            icon: 'el-icon-full-screen'
          }
        ]
      },
      sensorOptRight: {
        snsOptInd: 0,
        sensorInfo: [
          {
            id: 1,
            label: 'CAM1',
            icon: 'el-icon-full-screen'
          }
        ]
      },
      screenIcon: 'iconfont icon-fullscreen-enter',
      isAiEnable: false,
      isSearchImg: false,
      isFullScreen: false,
      isForceStop: false,
      autoSnapshotEnable: true,
      manualSnapshotEnable: true,
      flashEnable: true,
      ezoomEnable: false,
      aenc_enable: false,
      aenc_type: 'aac',
      aenc_sample_rate: 160000,
      aenc_num_channels: 1,
      isTalking: false,
      isMuted: true,
      wsCapture: [null, null], // auto capture, snapshot capture
      timerCapture: [undefined, undefined],
      lastRecvDataTime: [0, 0],
      wsEvents: null,
      timerEvents: undefined,
      curImgInd: {
        fh: [0, 0],  // face, body
        hcvfp: [0, 0, 0, 0, 0], // face, body, vechicle, cycle, plate
        snapshot: 0,
        flash: 0
      },
      max_try_num: 3,
      showViewer: false,
      full_scr_id: undefined,
      isEnable3ASR: true,
      ezoomMouseDown: false,
      ezoom: [
        {
          ezoom_ratio: 0,
          center_offset_x: 0,
          center_offset_y: 0
        },
        {
          ezoom_ratio: 0,
          center_offset_x: 0,
          center_offset_y: 0
        }
      ]
    }
  },
  created() {
    console.log('preview created ++')
    this.getInfo(false, 0)

    for (var i = 0; i < this.image_count; i++) {
      this.pushFHImages.face.push({ snsId: 0, type: 3, src: '', date: '', info: { gender: '--', age: '--', mask: '--', info: '--' } })
      this.pushFHImages.face.push({ snsId: 0, type: 3, src: '', date: '', info: { gender: '--', age: '--', mask: '--', info: '--' } })
      this.pushFHImages.body.push({ snsId: 0, type: 0, src: '', date: '', info: {} })
      this.pushFHImages.body.push({ snsId: 0, type: 0, src: '', date: '', info: {} })

      this.pushHCVFPImages.face.push({ snsId: 0, type: 3, src: '', date: '', info: {} })
      this.pushHCVFPImages.body.push({ snsId: 0, type: 0, src: '', date: '', info: {} })
      this.pushHCVFPImages.vechicle.push({ snsId: 0, type: 1, src: '', date: '', info: {} })
      this.pushHCVFPImages.cycle.push({ snsId: 0, type: 2, src: '', date: '', info: {} })
      this.pushHCVFPImages.plate.push({ snsId: 0, type: 4, src: '', date: '', info: { number: '--', color: '--' } })

      this.pushSnapshots.img.push({ snsId: 0, type: 5, src: '', date: '', info: {} })

      this.pushSnapshots.flash.push({ snsId: 0, type: 6, src: '', date: '', info: { pts: '--', seqnum: '--', userdata: '--'} })
    }

    this.$nextTick(function () {
      this.resizeCanvas()
    })

    console.log('preview created --')
  },
  mounted() {
    console.log('preview mounted ++')
    const _this = this
    if (screenfull.isEnabled) {
      screenfull.on('change', () => {
        _this.checkFull()
      })
    }

    window.addEventListener('visibilitychange', () => {
      if (document.visibilityState === "visible") {
        for (var i = 0; i < this.streamOptions.length; i++) {
        if (this.streamOptions[i].wfsObj && this.streamOptions[i].showVideoStatus) {
          this.streamOptions[i].wfsObj.playerSeekNow()
        }
        }
      } else {
        this.onStopTalk();
      }

    })

    console.log('preview mounted --')
  },
  destroyed() {
    this.stopMedia()
  },
  methods: {
    startMedia() {
      this.startPlay()
      this.startCapture()
      this.startRecvEvents()
    },
    stopMedia() {
      this.isForceStop = true
      for (var i = 0; i < this.sns_num; i++) {
        this.stopEncInfoQuery(i)
        this.stop(i)
      }

      this.onStopTalk();
      this.stopCatpure()
      this.stopRecvEvents()
      screenfull.off('change', () => { })
    },
    async startPlay() {
      if (this.sns_num === 1) {
        await this.startSensorPlay(0)
      } else if (this.sns_num >= 2) {
        let CurLeftSnsId = this.sensorOptLeft.sensorInfo[this.sensorOptLeft.snsOptInd].id
        await this.startSensorPlay(CurLeftSnsId)
        let CurRightSnsId = this.sensorOptRight.sensorInfo[this.sensorOptRight.snsOptInd].id
        await this.startSensorPlay(CurRightSnsId)
      }
    },
    startSensorPreview(snsId) {
      this.startSensorPlay(snsId)
      this.startCapture()
    },
    stopSensorPreview(snsId) {
      this.isForceStop = true
      this.stopEncInfoQuery(snsId)
      this.stop(snsId)
      this.stopCatpure()
    },
    async startSensorPlay(snsId) {
      var is_play_ok = false
      for (var i = 0; i < this.max_try_num; i++) {
        if (is_play_ok) {
          break
        }
        try {
          // call pay in changeStream
          await this.changeStream(snsId, this.streamOptions[snsId].streamInd, true, true)
          is_play_ok = true
        } catch (err) {
          is_play_ok = false
          if (i == this.max_try_num - 1) {
            console.log('startSensorPlay catch except: ' + err)
          }
        }
      }
      if (!is_play_ok) {
        this.$message.error('Network request stream failed')
      }
    },
    play(snsId) {
      console.log('play++, snsid: ' + snsId)
      try {
        this.stop(snsId)
        let sizeOptionInd = this.streamOptions[snsId].sizeOptionInd
        let streamInd = this.streamOptions[snsId].streamInd
        let mediaFPS = this.streamOptions[snsId].videoInfo[streamInd].mediaFPS
        this.changeSizeOption(snsId, sizeOptionInd)

        const tokenStr = window.sessionStorage.getItem('token')

        if (Wfs.isSupported()) {
          let config = {
            wsMinPacketInterval: 2000,
            wsMaxPacketInterval: 8000,
            liveMaxLatency: 0.5,
            fps: mediaFPS,
            recordId: snsId,
            recordInterval: 10000
          };
          let displayInfo = this.getDisplayInfo(snsId);
          console.log('new wfs obj, sns id: ' + snsId + ', mediaType: ' + displayInfo.mediaType);

          this.streamOptions[snsId].wfsObj = new Wfs(config)
          let media = {
            video: displayInfo.video,
            ctx: displayInfo.mjpeg
          }
          let audioInfo = null;
          // just support AENC on video 0.
          if (this.aenc_enable && snsId == 0) {
            audioInfo = {
              websocketName: 'ws/audio_' + snsId + '?token=' + tokenStr,
              mediaType: this.aenc_type,
              sampleRate: this.aenc_sample_rate,
              numChannels: this.aenc_num_channels,
              bitWidth: this.aenc_bit_width,
            };
          }

          this.streamOptions[snsId].wfsObj.attachMedia(media, 'ch' + snsId, displayInfo.mediaType, 'ws/preview' + '?token=' + tokenStr + '&&sns_id=' + snsId, displayInfo.extra, audioInfo)
          this.streamOptions[snsId].wfsObj.on(Wfs.Events.ERROR, (eventName, data) => (this.handleWfsEvent(snsId, eventName, data)))

          if (this.streamOptions[snsId].isRecStart && displayInfo.showVideo) {
            this.streamOptions[snsId].isRecStart = false
            this.onRecording(snsId)
          }

          if (displayInfo.mediaType == 'MJPEG') {
            this.resizeCanvas()
          }

          this.startEncInfoQuery(snsId)
        } else {
          console.log('wfs is not supported')
        }
      } catch (err) {
        console.log('play catch except: ' + err)
      }
      console.log('play--, snsId: ' + snsId)
    },
    stop(snsId) {
      console.log('stop++, sns id: ' + snsId)
      if (this.streamOptions[snsId].wfsObj != null) {
        if (this.streamOptions[snsId].isRecStart) {
          this.streamOptions[snsId].wfsObj.stop_recording()
          if (!this.streamOptions[snsId].isWfsError) {
            this.streamOptions[snsId].isRecStart = false
            this.streamOptions[snsId].recIcon = 'iconfont icon-record-start'
          }
        }
        this.streamOptions[snsId].wfsObj.destroy()
        this.streamOptions[snsId].wfsObj = null
      }
      console.log('stop--, sns id: ' + snsId)
    },
    getMediatype(codec_type) {
      if (codec_type === 0) {
        return 'H264Raw'
      } else if (codec_type === 1) {
        return 'MJPEG'
      } else if (codec_type === 2) {
        return 'H265Raw'
      }
      return 'H264Raw'
    },
    getDisplayInfo(snsId) {
      const videos = [document.getElementById('myVideo0'), document.getElementById('myVideo1')]
      const mjpegs = [document.getElementById('myMjpeg0'), document.getElementById('myMjpeg1')]
      let streamInd = this.streamOptions[snsId].streamInd
      let mediaType = this.streamOptions[snsId].videoInfo[streamInd].mediaType
      this.streamOptions[snsId].mjpegCtx.sizeOptionInd = this.streamOptions[snsId].sizeOptionInd
      let extra = this.streamOptions[snsId].mjpegCtx
      let showVideo = Wfs.canPlayByVideo(mediaType)
      this.streamOptions[snsId].showVideoStatus = showVideo
      return { video: videos[snsId % 2], mjpeg: mjpegs[snsId % 2], mediaType: mediaType, extra: extra, showVideo: showVideo };
    },
    changeSizeOption0(command) {
      this.changeSizeOption(this.sensorOptLeft.sensorInfo[this.sensorOptLeft.snsOptInd].id, Number(command))
    },
    changeSizeOption1(command) {
      this.changeSizeOption(this.sensorOptRight.sensorInfo[this.sensorOptRight.snsOptInd].id, Number(command))
    },
    changeSizeOption(snsId, sizeOptionInd) {
      const videos = [document.getElementById('myVideo0'), document.getElementById('myVideo1')]
      this.streamOptions[snsId].sizeOptionInd = sizeOptionInd
      if (this.streamOptions[snsId].sizeOptionInd === 0) {
        videos[snsId % 2].style.setProperty('object-fit', 'fill', 'important')
      } else {
        videos[snsId % 2].style.setProperty('object-fit', 'contain', 'important')
      }
      console.log('sns ' + snsId + ' old objectFit: ', videos[snsId % 2].style.objectFit)
      this.streamOptions[snsId].mjpegCtx.sizeOptionInd = this.streamOptions[snsId].sizeOptionInd
      window.localStorage.setItem('size_' + snsId, this.streamOptions[snsId].sizeOptionInd)
    },
    async changeStream0(command, replay = true, changeData = true) {
      let curSnsId = this.sensorOptLeft.sensorInfo[this.sensorOptLeft.snsOptInd].id
      await this.changeStream(curSnsId, Number(command), replay, changeData)
    },
    async changeStream1(command, replay = true, changeData = true) {
      let curSnsId = this.sensorOptRight.sensorInfo[this.sensorOptRight.snsOptInd].id
      await this.changeStream(curSnsId, Number(command), replay, changeData)
    },
    async changeStream(snsId, streamInd, replay = true, changeData = true) {
      // request http to send this configure to server
      console.log('video ' + snsId + ' change stream to ' + this.streamOptions[snsId].videoInfo[streamInd].id)

      const { data: res } = await this.$http.post('preview/stream', {
        src_id: snsId,
        stream: this.streamOptions[snsId].videoInfo[streamInd].id
      })
      console.log('post stream return: ', res)
      if (!res || res.meta.status !== 200) return this.$message.error('Change stream failed')
      if (changeData) {
        this.streamOptions[snsId].streamInd = streamInd
        let mediaType = this.streamOptions[snsId].videoInfo[streamInd].mediaType;
        this.streamOptions[snsId].recEnable = Wfs.canPlayByVideo(mediaType);
        window.localStorage.setItem('stream_' + snsId, this.streamOptions[snsId].videoInfo[streamInd].id)
      }
      this.clearEncInfo(snsId)
      if (replay) {
        this.play(snsId)
      }
    },
    async changeSensorOptLeft(command) {
      let curSnsId = this.sensorOptLeft.sensorInfo[this.sensorOptLeft.snsOptInd].id
      this.stop(curSnsId)
      this.stopEncInfoQuery(curSnsId)
      let streamInd = this.streamOptions[curSnsId].streamInd
      let snsId = Number(command)
      console.log('changeSensorOptLeft sns id: ' + snsId + " stream id: " + streamInd)
      for (var i = 0; i < this.sensorOptLeft.sensorInfo.length; i++) {
        if (snsId == this.sensorOptLeft.sensorInfo[i].id) {
          this.sensorOptLeft.snsOptInd = i
          window.localStorage.setItem('snsOptLeftInd', i)
          break
        }
      }

      await this.changeStream(snsId, streamInd, true, true)
    },
    async changeSensorOptRight(command) {
      let curSnsId = this.sensorOptRight.sensorInfo[this.sensorOptRight.snsOptInd].id
      this.stop(curSnsId)
      this.stopEncInfoQuery(curSnsId)
      let streamInd = this.streamOptions[curSnsId].streamInd

      let snsId = Number(command)
      console.log('changeSensorOptRight sns id: ' + snsId + " stream id: " + streamInd)
      for (var i = 0; i < this.sensorOptRight.sensorInfo.length; i++) {
        if (snsId == this.sensorOptRight.sensorInfo[i].id) {
          this.sensorOptRight.snsOptInd = i
          window.localStorage.setItem('snsOptRightInd', i)
          break
        }
      }

      await this.changeStream(snsId, streamInd, true, true)
    },
    async recordFaceinfo(snsId) {
      // request http to send this configure to server
      console.log('record faceinfo, sns id: ' + snsId)
      this.stop(snsId)
      const { data: res } = await this.$http.post('preview/faceinfo', {
        src_id: snsId,
        faceinfo: true
      })
      this.play(snsId)
      console.log('post record faceinfo: ', res)
      if (res.meta.status !== 200) {
        return this.$message.error('Input facial data failed')
      }
      else {
        if (res.data.faceinfo.status == 0) {
          return this.$message.success('Input facial data successful')
        }
        else if (res.data.faceinfo.status == 2) {
          return this.$message.warning('Not support')
        }
        else if (res.data.faceinfo.status == 3) {
          return this.$message.warning('data is loading, please try again later')
        }
        else {
          return this.$message.error('Input facial data failed, please retry')
        }
      }
    },
    initStreamOptions(snsNum) {
      for (var k = 2; k < snsNum; k++) {
        var _item = Object.assign({}, this.streamOptions[0])
        this.streamOptions.push(_item)
      }
    },
    initSensorOptInd(snsNum) {
      var ind = window.localStorage.getItem('snsOptLeftInd')
      if (ind == undefined || snsNum < 4) {
        this.sensorOptLeft.snsOptInd = 0
      }
      else {
        ind = parseInt(ind)
        if (ind > (snsNum / 2 - 1)) {
          this.sensorOptLeft.snsOptInd = 0
        }
        else {
          this.sensorOptLeft.snsOptInd = ind
        }
      }

      var ind2 = window.localStorage.getItem('snsOptRightInd')
      if (ind2 == undefined || snsNum < 4 ) {
        this.sensorOptRight.snsOptInd = 0
      }
      else {
        ind2 = parseInt(ind2)
        if (ind2 > (snsNum / 2 - 1)) {
          this.sensorOptRight.snsOptInd = 0
        }
        else {
          this.sensorOptRight.snsOptInd = ind2
        }
      }
    },
    async getInfo(changeSNSVideo, snsId) {
      try {
        const { data: res } = await this.$http.get('preview/info')
        console.log('preview info get return: ', res)
        if (res.meta.status === 200) {
          this.sns_num = res.data.info.sns_num
          this.initStreamOptions(this.sns_num)
          this.sensorOptLeft.sensorInfo.splice(1)
          this.sensorOptRight.sensorInfo.splice(1)
          this.ezoom.splice(2)
          for (var k = 0; k < this.sns_num; k++) {
            this.streamOptions[k].videoInfo = []
            for (var i = 0; i < res.data.info.stream_list[k].length; i++) {
              var stream_id = res.data.info.stream_list[k][i]
              var _item = {
                id: stream_id,
                label: (0 === stream_id) ? ('Main Stream0') : ('Sub Stream' + stream_id + ''),
                icon: 'el-icon-full-screen',
                mediaType: this.getMediatype(res.data.info.sns_codec[k][i]),
                mediaFPS: res.data.info.sns_video_fps[k][i]
              }
              this.streamOptions[k].videoInfo.push(_item)
              if (0 == k) {
                this.streamOptions[k].trigger_attr.trigger_enable = res.data.info.trigger_enable
              }
            }

            if (k >= 2) {
              if (k % 2 == 0) {
                var _item = {
                  id: k,
                  label: 'CAM' + k,
                  icon: 'el-icon-full-screen'
                }
                this.sensorOptLeft.sensorInfo.push(_item)
              } else {
                var _item = {
                  id: k,
                  label: 'CAM' + k,
                  icon: 'el-icon-full-screen'
                }
                this.sensorOptRight.sensorInfo.push(_item)
              }

              // ezoom
              var _item = {
                ezoom_ratio: res.data.info.ezoom[k].ezoom_ratio,
                center_offset_x: res.data.info.ezoom[k].center_offset_x,
                center_offset_y: res.data.info.ezoom[k].center_offset_y
              }
              this.ezoom.push(_item)
            }
          }

          this.initSensorOptInd(this.sns_num);

          this.manualSnapshotEnable = res.data.info.snap_shot_enable
          this.autoSnapshotEnable = res.data.info.auto_capture_enable
          this.flashEnable = res.data.info.flash_enable
          this.ezoomEnable = res.data.info.ezoom_enable
          this.aenc_enable = res.data.info.aenc_enable
          this.aenc_type = res.data.info.aenc_type;
          this.aenc_sample_rate = res.data.info.aenc_sample_rate;
          this.aenc_bit_width = res.data.info.aenc_bit_width;

          /* ai detection info */
          this.isSearchImg = res.data.info.searchimg
          this.isAiEnable = res.data.info.ai_enable
          this.detect_mode = res.data.info.detect_mode
          console.log('detect_mode: ' + this.detect_mode)
          if (!this.detect_mode) {
            this.detect_mode = 'facehuman'
            console.log('set detect_mode to default: ' + this.detect_mode)
          }

          for (var j = 0; j < this.sns_num; j++) {
            var ind = window.localStorage.getItem('stream_' + j)
            var max_ind = res.data.info.stream_list[j].length - 1
            if (ind == undefined) {
              this.streamOptions[j].streamInd = Math.min(1, max_ind)
            }
            else {
              ind = parseInt(ind)
              if (ind > max_ind) {
                this.streamOptions[j].streamInd = Math.min(1, max_ind)
              }
              else {
                this.streamOptions[j].streamInd = ind
              }
            }

            ind = window.localStorage.getItem('size_' + j)
            if (ind == undefined) {
              this.streamOptions[j].sizeOptionInd = 0
            }
            else {
              this.streamOptions[j].sizeOptionInd = parseInt(ind)
            }

            this.streamOptions[j].mjpegCtx.sizeOptionInd = this.streamOptions[j].sizeOptionInd

            this.streamOptions[j].showVideoStatus = Wfs.canPlayByVideo(this.streamOptions[j].videoInfo[this.streamOptions[j].streamInd].mediaType)

            this.streamOptions[j].recEnable = (this.streamOptions[j].showVideoStatus) ? true : false
          }

          console.log("this.sns_num:" + this.sns_num + " sns select: " + this.sensor_select)

          if (!changeSNSVideo) {
            this.startMedia()
          } else {
            this.startSensorPreview(snsId)
          }
        }
      } catch (error) {
        console.log('getInfo exception: ' + error)
        this.$message.error('Failed to get information')
      }
    },
    recResultCallback(result, recordId) {
      try {
        this.streamOptions[recordId].recEnable = true
        if (result) {
          this.streamOptions[recordId].isRecStart = true
          this.streamOptions[recordId].recIcon = 'iconfont icon-record-stop'
        }
        else {
          this.$message.error('Start record[' + recordId + '] failed')
        }
      } catch (error) {
        this.$message.error('Start record[' + recordId + '] failed')
      }
    },
    recStateCallback(state, recordId) {
      try {
        this.streamOptions[recordId].recEnable = state
      } catch (error) {
        this.$message.error('Get record[' + recordId + '] status failed')
      }
    },
    async doTriggerFlash(snsId) {
      console.log('doTriggerFlash.', snsId)
      try {
        this.streamOptions[snsId].trigger_attr.trigger = true
        const { data: res } = await this.$http.post('preview/trigger', {
          src_id: snsId,
          trigger_attr: this.streamOptions[snsId].trigger_attr
        })

        console.log('trigger get return: ', res)
        if (res.meta.status === 200) {
          this.$message.success('Trigger successful')
        } else {
          this.$message.error('Trigger failed')
        }
      } catch (error) {
        this.$message.error('Trigger failed')
      }
      this.streamOptions[snsId].trigger_attr.trigger = false
    },
    async onSnapshot(snsId) {
      let stream_id = this.streamOptions[snsId].videoInfo[this.streamOptions[snsId].streamInd].id;
      try {
        const { data: res } = await this.$http.post('preview/snapshot', {
          src_id: snsId,
          stream: stream_id
        })
        if (res.meta.status === 200) {
          console.log('send snapshot cmd success')
        }
      } catch (error) {
        console.log('onSnapshot exception: ' + error)
        this.$message.error('Send capture command failed')
      }
    },
    onEZoomMousedown() {
      this.ezoomMouseDown = true;
      window.onmouseup = () => {
        this.ezoomMouseDown = false;
        window.onmouseup = null;
      };
    },
    async onEZoom(snsId) {
      if (!this.ezoomMouseDown) {
        return
      }
      let stream_id = this.streamOptions[snsId].videoInfo[this.streamOptions[snsId].streamInd].id;
      try {
        const { data: res } = await this.$http.post('preview/ezoom', {
          src_id: snsId,
          stream: stream_id,
          ezoom: this.ezoom[snsId]
        })
        console.log('ezoom get return: ', res)
        if (res.meta.status === 200) {
          console.log('send ezoom cmd success')
        }
      } catch (error) {
        this.$message.error('Send ezoom command failed')
      }
    },
    async onRecording(snsId) {
      if (this.streamOptions[snsId].wfsObj) {
        var is_rec_start = this.streamOptions[snsId].isRecStart
        if (!is_rec_start) {
          this.streamOptions[snsId].recEnable = false
          var rec_threshold_web = window.localStorage.getItem('rec_threshold_web')
          if (rec_threshold_web == undefined) {
            rec_threshold_web = 1024
          }

          console.log('sns[' + snsId + '] recording - threshold: ' + rec_threshold_web + ' MB')
          this.streamOptions[snsId].wfsObj.start_recording(rec_threshold_web * 1024 * 1024,
            this.recResultCallback,
            this.recStateCallback)
        } else {
          this.streamOptions[snsId].wfsObj.stop_recording()
          this.streamOptions[snsId].isRecStart = false
          this.streamOptions[snsId].recIcon = 'iconfont icon-record-start'
        }
      }
    },
    async getEncInfo(snsId) {
      try {
        let streamId = this.streamOptions[snsId].videoInfo[this.streamOptions[snsId].streamInd].id
        let mediaType = this.streamOptions[snsId].videoInfo[this.streamOptions[snsId].streamInd].mediaType
        const { data: res } = await this.$http.get('preview/assist', {
          params: {
            src_id: snsId,
            stream: streamId
          }
        })
        // console.log('preview enc info get return: ', res)
        if (res.meta.status === 200) {
          let mediaTypeStr = ''
          switch (mediaType) {
            case 'MJPEG':
              mediaTypeStr = 'MJPEG'
              break;
            case 'H265Raw':
              mediaTypeStr = 'H265'
              break;
            case 'H264Raw':
            default:
              mediaTypeStr = 'H264'
              break;
          }
          if (this.sensor_select === 1) {
            this.streamOptions[snsId].encInfo = 'Encoder: ' + mediaTypeStr + '\xa0\xa0'
            + 'Resolution: ' + res.data.assist_res + '\xa0\xa0'
            + 'Bitrate: ' + res.data.assist_bitrate

          } else {
            this.streamOptions[snsId].encInfo = 'CAM' + snsId + '\xa0\xa0'
            + 'Encoder: ' + mediaTypeStr + '\xa0\xa0'
            + 'Resolution: ' + res.data.assist_res + '\xa0\xa0'
            + 'Bitrate: ' + res.data.assist_bitrate
          }

        }
      } catch (error) {
        console.log('getEncInfo exception: ' + error)
        this.$message.error('Failed to get information')
      }
    },
    async clearEncInfo(snsId) {
      this.streamOptions[snsId].encInfo = ''
    },
    async onStartTalk() {
      if (this.isTalking) {
        this.onStopTalk()
        return
      }

      this.startAudioComm();
      this.isForceStopTalk = false
      if (!this.rc) {
        this.rc = new Recorderx({
          recordable: true,
          sampleRate: this.aenc_sample_rate,
          sampleBits: (this.aenc_bit_width + 1) * 8,
          bufferSize: 2048,
        })
      }

      if (this.rc && this.rc.state === RECORDER_STATE.READY) {
        this.isTalking = true
        this.rc.clear()
        this.rc.start((data) => {
          if (this.isTalking) {
            const pcm = this.rc.getRecord({ encodeTo: ENCODE_TYPE.PCM, compressible: true })
            this.rc.clear()
            let wsTalk = this.wsTalk;
            if (this.isTalking && wsTalk && wsTalk.readyState === WebSocket.OPEN && pcm && pcm.byteLength) {
              wsTalk.send(pcm)
              console.log('ws talk send data len: ' + pcm.byteLength)
            }
          }
        })
          .then(() => {
            console.log('start talk')
          })
          .catch((error) => {
            console.log('talk failed.', error)
          })
      }
    },
    async onStopTalk() {
      this.isForceStopTalk = true
      if (this.rc && this.rc.state === RECORDER_STATE.RECORDING) {
        this.rc.pause()
      }
      if (this.isTalking) {
        this.stopAudioComm()
        this.isTalking = false
        this.rc = null
        console.log('stop talk')
      }
    },
    async onMute() {
      this.isMuted = !this.isMuted
      var video0 = document.getElementById('myVideo0')
      video0.muted = this.isMuted
    },
    startEncInfoQuery(snsId) {
      this.stopEncInfoQuery(snsId)
      this.streamOptions[snsId].timerEncInfo = setInterval(() => {
        this.getEncInfo(snsId)
      }, 1000)
    },
    stopEncInfoQuery(snsId) {
      if (this.streamOptions[snsId].timerEncInfo) {
        clearInterval(this.streamOptions[snsId].timerEncInfo)
        this.streamOptions[snsId].timerEncInfo = undefined
      }
    },
    async change3ASR (changeData = true) {
      // request http to send this configure to server
      if (changeData) {
        console.log('enable 3ASR:' + !this.isEnable3ASR)
        const { data: res } = await this.$http.post('preview/sync_ratio_3a', { sr3a: !this.isEnable3ASR })
        console.log('post enable 3ASR return: ', res)
        if (res.meta.status !== 200) return this.$message.error('Set 3ASR status failed')
        this.isEnable3ASR = !this.isEnable3ASR
      } else {
        console.log('enable 3ASR:' + this.isEnable3ASR)
        const { data: res } = await this.$http.post('preview/sync_ratio_3a', { sr3a: this.isEnable3ASR })
        console.log('post enable 3ASR return: ', res)
        if (res.meta.status !== 200) return this.$message.error('Set 3ASR status failed')
      }
    },
    doScreen(snsId) {
      const element = (snsId % 2 == 0) ? document.getElementById('video_container_0') : document.getElementById('video_container_1')
      if (this.isFullScreen) {
        screenfull.exit(element)
        this.screenIcon = 'iconfont icon-fullscreen-enter'
        this.resumePlay(snsId)
      } else {
        screenfull.request(element)
        this.screenIcon = 'iconfont icon-fullscreen-exit'
        this.full_scr_id = snsId
      }
      this.isFullScreen = !this.isFullScreen
    },
    checkFull() {
      if (!screenfull.isFullscreen) {
        this.isFullScreen = false
        this.screenIcon = 'iconfont icon-fullscreen-enter'
        this.resumePlay(this.full_scr_id)
      }
    },
    resumePlay(snsId) {
      if (this.streamOptions[snsId].showVideoStatus) {
        this.streamOptions[snsId].wfsObj.playerSeekNow()
      }
    },
    handleWfsEvent(snsId, eventName, data) {
      try {
        if (data.fatal) {
          this.streamOptions[snsId].isWfsError = true
          console.warn('sns[' + snsId + '] fatal error :' + data.details)
          switch (data.type) {
            case Wfs.ErrorTypes.MEDIA_ERROR:
              console.log('sns[' + snsId + '] a media error occurred')
              break
            case Wfs.ErrorTypes.NETWORK_ERROR:
              console.log('sns[' + snsId + '] a network error occurred')
              break
            default:
              console.log('sns[' + snsId + '] an unrecoverable error occurred')
              break
          }
          this.play(snsId)
        }
      } catch (error) {
        console.log('sns[' + snsId + '] handleWfsEvent catch except: ' + error)
      }
    },
    startCaptureById(capId) {
      if ((capId === 0 && !this.autoSnapshotEnable && !this.flashEnable) || (capId === 1 && !this.manualSnapshotEnable && !this.flashEnable)) {
        return
      }

      this.stopCatpureById(capId)
      const tokenStr = window.sessionStorage.getItem('token')
      var uri = 'ws://' + window.location.host + '/ws/capture_' + capId + '?token=' + tokenStr
      var protocol = 'binary'
      this.wsCapture[capId] = new WebSocket(uri, protocol)

      this.wsCapture[capId].onopen = () => {
        console.log('cap[' + capId + '] ws capture connected')
        this.createCaptureTimer(capId)
      }
      this.wsCapture[capId].onclose = (err) => {
        console.log('cap[' + capId + '] ws capture disconnected', err)
        if (!this.isForceStop) {
          this.startCaptureById(capId)
        }
      }
      this.wsCapture[capId].onmessage = (event) => {
        if (typeof (event.data) === 'string') {
          console.log('cap[' + capId + '] capture got a message: ' + event.data)
        } else {
          // console.log('got a wscapture image data, data-type:' + typeof (event.data) )
          try {
            this.lastRecvDataTime[capId] = performance.now()
            if (capId === 0) {
              var reader = new FileReader()
              reader.onload = (evt) => {
                if (evt.target.readyState === FileReader.DONE) {
                  this.parseJpgDataWithHead(evt.target.result)
                }
              }
              reader.readAsArrayBuffer(event.data)
            }
            else if (capId === 1) {
              const blob = new Blob([event.data], { type: 'image/jpeg' })
              const fileName = 'snapshot_' + this.getCurrentTime() + '.jpg'
              saveAs(blob, fileName)
            }
          } catch (err) {
            console.log('cap[' + capId + '] capture image catch except: ' + err)
          }
        }
      }
      this.wsCapture[capId].onerror = (err) => {
        console.log('cap[' + capId + '] capture occure error', err)
      }
    },
    startCapture() {
      for (var i = 0; i < 2; i++) { //0: auto; 1: manual
        this.startCaptureById(i)
      }
    },
    startAudioComm() {
      this.wsTalkBufList = []
      this.wsTalkBufSize = 0
      const tokenStr = window.sessionStorage.getItem('token')
      var uri = 'ws://' + window.location.host + '/ws/talk?token=' + tokenStr
      var protocol = 'binary'
      this.wsTalk = new WebSocket(uri, protocol)
      this.wsTalk.onopen = () => {
        console.log('ws talk connected')
      }
      this.wsTalk.onclose = (err) => {
        console.log('ws talk disconnected: ', err)
        if (this.isForceStopTalk) {
          this.onStopTalk()
        } else {
          this.wsTalk = null
          this.startAudioComm()
        }
      }
      this.wsTalk.onmessage = (event) => {
        if (typeof (event.data) === 'string') {
          console.log('got a wscapture message: ' + event.data)
        }
      }
      this.wsTalk.onerror = (err) => {
        console.log('ws talk occure error: ', err)
      }
    },
    stopAudioComm() {
      if (this.wsTalk) {
        this.wsTalk.close()
        this.wsTalk = null
      }
    },
    stopCatpureById(capId) {
      if ((capId === 0 && !this.autoSnapshotEnable && !this.flashEnable) || (capId === 1 && !this.manualSnapshotEnable && !this.flashEnable)) {
        return
      }
      this.clearCaptureTimer(capId)
      if (this.wsCapture[capId]) {
        this.wsCapture[capId].close()
      }
    },
    stopCatpure() { //0: auto; 1: manual
      for (var i = 0; i < 2; i++) {
        this.stopCatpureById(i)
      }
    },
    createCaptureTimer(capId) {
      this.clearCaptureTimer(capId)
      this.timerCapture[capId] = setInterval(() => {
        if (this.wsCapture[capId] && this.wsCapture[capId].readyState === WebSocket.OPEN) {
          this.wsCapture[capId].send("keep-alive")
        }
      }, 3000)
    },
    clearCaptureTimer(capId) {
      if (this.timerCapture[capId]) {
        clearInterval(this.timerCapture[capId])
        this.timerCapture[capId] = undefined
      }
    },
    startRecvEvents() {
      console.log('startRecvEvents ++')
      this.stopRecvEvents()
      const tokenStr = window.sessionStorage.getItem('token')
      var uri = 'ws://' + window.location.host + '/ws/events?token=' + tokenStr
      var protocol = 'binary'
      this.wsEvents = new WebSocket(uri, protocol)
      this.wsEvents.onopen = () => {
        console.log('ws events connected')
        this.createEventstimer()
      }
      this.wsEvents.onclose = (err) => {
        console.log('ws events disconnected', err)
        if (!this.isForceStop) {
          this.startRecvEvents()
        }
      }
      this.wsEvents.onmessage = (event) => {
        if (typeof (event.data) === 'string') {
          console.log('got a ws events message: ' + event.data)
        } else {
          try {
            var reader = new FileReader()
            reader.onload = (evt) => {
              if (evt.target.readyState === FileReader.DONE) {
                // console.log('got a event: ' + evt.target.result)
                var eventObj = JSON.parse(evt.target.result)
                for (var i = 0; i < eventObj.events.length; i++) {
                  if (eventObj.events[i].type === 1) {
                    this.eventList.unshift({ type: 'MD', date: eventObj.events[i].date, sensor: eventObj.events[i].sensor })
                  } else if (eventObj.events[i].type === 2) {
                    this.eventList.unshift({ type: 'OD', date: eventObj.events[i].date, sensor: eventObj.events[i].sensor })
                  } else if (eventObj.events[i].type === 3) {
                    this.eventList.unshift({ type: 'SCD', date: eventObj.events[i].date, sensor: eventObj.events[i].sensor })
                  } else if (eventObj.events[i].type === 0) {
                    console.log('got a video change event: ' + evt.target.result)
                    this.resetVideoStream(eventObj.events[i].sensor)
                  }
                }

                if (this.eventList.length > 32) {
                  this.eventList.splice(32, this.eventList.length - 32)
                }
              }
            }
            reader.readAsText(event.data)
          } catch (err) {
            console.log('ws events catch except: ' + err)
          }
        }
      }
      this.wsEvents.onerror = (err) => {
        console.log('ws events occure error', err)
      }
      console.log('startRecvEvents --')
    },
    resetVideoStream(snsId) {
      this.stopSensorPreview(snsId)
      this.getInfo(true, snsId)
    },
    stopRecvEvents() {
      console.log('stopRecvEvents ++')
      this.clearEventstimer()
      if (this.wsEvents) {
        this.wsEvents.close()
      }
      console.log('stopRecvEvents --')
    },
    createEventstimer() {
      this.clearEventstimer()
      this.timerEvents = setInterval(() => {
        if (this.wsEvents && this.wsEvents.readyState === WebSocket.OPEN) {
          this.wsEvents.send("keep-alive")
        }
      }, 3000)
    },
    clearEventstimer() {
      if (this.timerEvents) {
        clearInterval(this.timerEvents)
        this.timerEvents = undefined
      }
    },
    parseJpgDataWithHead(jpgData) {
      try {
        var jpgHdr = new Uint8Array(jpgData.slice(0, 16))
        // console.log('-->jpg header: ' + jpgHdr)

        if (jpgHdr && jpgHdr[0] === 0x41 && jpgHdr[1] === 0x58 && jpgHdr[2] === 0x49 && jpgHdr[3] === 0x54) {
          var hdrLen = this.bytesToIntLittleEndian(jpgHdr.subarray(4, 8))
          // console.log('ws parse jpg header: total-len=' + hdrLen)
          if (jpgHdr[8] === 0x4A && jpgHdr[9] === 0x53 && jpgHdr[10] === 0x4F && jpgHdr[11] === 0x4E) {
            var jsonLen = this.bytesToIntLittleEndian(jpgHdr.subarray(12, 16))
            // console.log('ws parse jpg header: json-len=' + jsonLen)
            var enc = new TextDecoder("utf-8");
            var jsonData = new Uint8Array(jpgData.slice(16, 16 + jsonLen - 1))
            var jsonStr = enc.decode(jsonData)
            // console.log('jpg info:' + jsonStr)
            var jpgInfo = JSON.parse(jsonStr)
            //console.log('jpg info - type:' + jpgInfo.type)

            var blob = new Blob([jpgData.slice(hdrLen)], { type: 'image/jpeg' });
            var url = (URL || webkitURL).createObjectURL(blob);

            if (jpgInfo && url) {
              // console.log('fill image info +++: ' + this.detect_mode)

              //JPEG_TYPE_BODY = 0,
              //JPEG_TYPE_VEHICLE,
              //JPEG_TYPE_CYCLE,
              //JPEG_TYPE_FACE,
              //JPEG_TYPE_PLATE,

              if (this.detect_mode === 'facehuman') {
                if (jpgInfo.type === 3) { // face
                  var index = this.curImgInd.fh[0]
                  this.pushFHImages.face[index].snsId = jpgInfo.snsId
                  this.pushFHImages.face[index].src = url
                  this.pushFHImages.face[index].date = this.getCurrentTime()
                  this.pushFHImages.face[index].info.gender = this.genderOptions[jpgInfo.attribute.face.gender]
                  this.pushFHImages.face[index].info.age = jpgInfo.attribute.face.age
                  this.pushFHImages.face[index].info.mask = this.respiratorOptions[jpgInfo.attribute.face.mask]
                  this.pushFHImages.face[index].info.info = jpgInfo.attribute.face.info
                  this.curImgInd.fh[0] = (index + 1) % this.pushFHImages.face.length

                  // this.currentImg.src = url + '?' + new Date().getTime()
                  // this.currentImg.src = url + '?t=' + Math.random()
                  this.currentImg.snsId = jpgInfo.snsId
                  this.currentImg.src = url
                  this.currentImg.type = 3
                  this.currentImg.date = this.pushFHImages.face[index].date
                  this.currentImg.info.gender = this.pushFHImages.face[index].info.gender
                  this.currentImg.info.age = this.pushFHImages.face[index].info.age
                  this.currentImg.info.mask = this.pushFHImages.face[index].info.mask
                  this.currentImg.info.info = this.pushFHImages.face[index].info.info

                } else if (jpgInfo.type === 0) { // body
                  var index = this.curImgInd.fh[1]
                  this.pushFHImages.body[index].snsId = jpgInfo.snsId
                  this.pushFHImages.body[index].src = url
                  this.pushFHImages.body[index].date = this.getCurrentTime()
                  this.pushFHImages.face[index].info = {}
                  this.curImgInd.fh[1] = (index + 1) % this.pushFHImages.body.length
                }
              } else if (this.detect_mode === 'hvcfp') {
                if (jpgInfo.type === 3) { // face
                  var index = this.curImgInd.hcvfp[0]
                  this.pushHCVFPImages.face[index].snsId = jpgInfo.snsId
                  this.pushHCVFPImages.face[index].src = url
                  this.pushHCVFPImages.face[index].date = this.getCurrentTime()
                  this.pushHCVFPImages.face[index].info = {}
                  this.curImgInd.hcvfp[0] = (index + 1) % this.pushHCVFPImages.face.length
                } else if (jpgInfo.type === 0) { // body
                  var index = this.curImgInd.hcvfp[1]
                  this.pushHCVFPImages.body[index].snsId = jpgInfo.snsId
                  this.pushHCVFPImages.body[index].src = url
                  this.pushHCVFPImages.body[index].date = this.getCurrentTime()
                  this.pushHCVFPImages.body[index].info = {}
                  this.curImgInd.hcvfp[1] = (index + 1) % this.pushHCVFPImages.body.length
                } else if (jpgInfo.type === 1) { // vechicle
                  var index = this.curImgInd.hcvfp[2]
                  this.pushHCVFPImages.vechicle[index].snsId = jpgInfo.snsId
                  this.pushHCVFPImages.vechicle[index].src = url
                  this.pushHCVFPImages.vechicle[index].date = this.getCurrentTime()
                  this.pushHCVFPImages.vechicle[index].info = {}
                  this.curImgInd.hcvfp[2] = (index + 1) % this.pushHCVFPImages.vechicle.length
                } else if (jpgInfo.type === 2) {
                  var index = this.curImgInd.hcvfp[3]
                  this.pushHCVFPImages.cycle[index].snsId = jpgInfo.snsId
                  this.pushHCVFPImages.cycle[index].src = url
                  this.pushHCVFPImages.cycle[index].date = this.getCurrentTime()
                  this.pushHCVFPImages.cycle[index].info = {}
                  this.curImgInd.hcvfp[3] = (index + 1) % this.pushHCVFPImages.cycle.length
                } else if (jpgInfo.type === 4) { // plate
                  var index = this.curImgInd.hcvfp[4]
                  this.pushHCVFPImages.plate[index].snsId = jpgInfo.snsId
                  this.pushHCVFPImages.plate[index].src = url
                  this.pushHCVFPImages.plate[index].date = this.getCurrentTime()
                  this.pushHCVFPImages.plate[index].info.number = jpgInfo.attribute.plate.number
                  this.pushHCVFPImages.plate[index].info.color = this.plateColorOptions[jpgInfo.attribute.plate.color]
                  this.curImgInd.hcvfp[4] = (index + 1) % this.pushHCVFPImages.plate.length

                  this.currentImg.snsId = jpgInfo.snsId
                  this.currentImg.src = url
                  this.currentImg.type = 4
                  this.currentImg.date = this.pushHCVFPImages.plate[index].date
                  this.currentImg.info.number = this.pushHCVFPImages.plate[index].info.number
                  this.currentImg.info.color = this.pushHCVFPImages.plate[index].info.color
                }
              }

              if (jpgInfo.type === 5) { // snapshot
                var index = this.curImgInd.snapshot
                this.pushSnapshots.img[index].snsId = jpgInfo.snsId
                this.pushSnapshots.img[index].src = url
                this.pushSnapshots.img[index].date = this.getCurrentTime()
                this.pushSnapshots.img[index].info = {}
                this.curImgInd.snapshot = (index + 1) % this.pushSnapshots.img.length
              } else if (jpgInfo.type === 6) { // flash
                var index = this.curImgInd.flash
                this.pushSnapshots.flash[index].snsId = jpgInfo.snsId
                this.pushSnapshots.flash[index].src = url
                this.pushSnapshots.flash[index].date = this.getCurrentTime()
                this.pushSnapshots.flash[index].info.pts = jpgInfo.attribute.pts
                this.pushSnapshots.flash[index].info.seqnum = jpgInfo.attribute.seqnum
                this.pushSnapshots.flash[index].info.userdata = jpgInfo.attribute.userdata
                this.curImgInd.flash = (index + 1) % this.pushSnapshots.flash.length
              }
            }
          }
        }
      } catch (err) {
        console.log('ws parse jpg with header catch except: ' + err)
      }
    },
    refreshMjpegCanvas(snsId) {
      var canvas = document.getElementById((snsId == 0) ? 'myMjpeg0' : 'myMjpeg1')
      if (canvas) {
        this.streamOptions[snsId].mjpegCtx.mjpeg_w = (snsId == 0) ? this.$refs.frtMain0.$el.offsetWidth : this.$refs.frtMain1.$el.offsetWidth
        this.streamOptions[snsId].mjpegCtx.mjpeg_h = (snsId == 0) ? this.$refs.frtMain0.$el.offsetHeight : this.$refs.frtMain1.$el.offsetHeight

        canvas.setAttribute("width", this.streamOptions[snsId].mjpegCtx.mjpeg_w)
        canvas.setAttribute("height", this.streamOptions[snsId].mjpegCtx.mjpeg_h)

        console.log('sns[' + snsId + ']: canvas_w=' + this.streamOptions[snsId].mjpegCtx.mjpeg_w + ',canvas_h=' + this.streamOptions[snsId].mjpegCtx.mjpeg_h)

        this.streamOptions[snsId].mjpegCtx.isDrawImg = false
        if (this.streamOptions[snsId].mjpegCtx.timerDraw) {
          clearInterval(this.streamOptions[snsId].mjpegCtx.timerDraw)
        }
        this.streamOptions[snsId].mjpegCtx.timerDraw = setInterval(() => {
          this.streamOptions[snsId].mjpegCtx.isDrawImg = true
          clearInterval(this.streamOptions[snsId].mjpegCtx.timerDraw)
        }, 300)
      }
    },
    resizeCanvas() {
      this.refreshMjpegCanvas(0)
      this.refreshMjpegCanvas(1)
    },
    getObjectFitSize(
      type = 0,
      containerWidth,
      containerHeight,
      imgWidth,
      imgHeight) {
      let radio = 1, sx = 0, sy = 0, swidth = imgWidth, sheight = imgHeight,
        x = 0, y = 0, width = containerWidth, height = containerHeight
      let cWHRatio = containerWidth / containerHeight
      let iWHRatio = imgWidth / imgHeight
      if (type === 0) { // fill
        if (iWHRatio >= cWHRatio) {
          radio = containerHeight / imgHeight
          sx = (imgWidth - containerWidth / radio) / 2
          swidth = containerWidth / radio
          sheight = imgHeight
        } else {
          radio = containerWidth / imgWidth
          sy = (imgHeight - containerHeight / radio) / 2
          swidth = imgWidth
          sheight = containerHeight / radio
        }
      } else if (type === 1) { // contain
        if (iWHRatio >= cWHRatio) {
          radio = containerWidth / imgWidth
          y = (containerHeight - imgHeight * radio) / 2
          height = imgHeight * radio;
        } else {
          radio = containerHeight / imgHeight
          x = (containerWidth - imgWidth * radio) / 2
          width = imgWidth * radio
        }
      }
      return {
        sx,
        sy,
        swidth,
        sheight,
        x,
        y,
        width,
        height,
      }
    },
    getCurrentTime() {
      var date = new Date()
      var month = this.zeroFill(date.getMonth() + 1)
      var day = this.zeroFill(date.getDate())
      var hour = this.zeroFill(date.getHours())
      var minute = this.zeroFill(date.getMinutes())
      var second = this.zeroFill(date.getSeconds())
      var ms = date.getMilliseconds()
      if (ms <= 9) {
        ms = '00' + ms
      } else if (ms <= 99) {
        ms = '0' + ms
      }
      var curTime = date.getFullYear() + '-' + month + '-' + day + ' ' + hour + ':' + minute + ':' + second + '.' + ms
      return curTime
    },
    zeroFill(i) {
      if (i >= 0 && i <= 9) {
        return '0' + i
      } else {
        return i
      }
    },
    bytesToIntLittleEndian(bytes) {
      var val = 0
      for (var i = bytes.length - 1; i >= 0; i--) {
        val += bytes[i]
        if (i != 0) {
          val = val << 8
        }
      }
      return val
    },
    handleViewerClose() {
      this.showViewer = false
    },
    look(imgInfo) {

      if ((this.detect_mode === 'facehuman' && imgInfo.type === 3) || (this.detect_mode === 'hvcfp' && imgInfo.type === 4)) {
        this.currentImg.snsId = imgInfo.snsId
        this.currentImg.type = imgInfo.type
        this.currentImg.src = imgInfo.src
        this.currentImg.date = imgInfo.date

        this.currentImg.info.age = imgInfo.info.age
        this.currentImg.info.gender = imgInfo.info.gender
        this.currentImg.info.mask = imgInfo.info.mask
        this.currentImg.info.info = imgInfo.info.info
        this.currentImg.info.number = imgInfo.info.number
        this.currentImg.info.color = imgInfo.info.color
      }

      this.viewerImg.snsId = imgInfo.snsId
      this.viewerImg.type = imgInfo.type
      this.viewerImg.src = imgInfo.src
      this.viewerImg.date = imgInfo.date
      this.viewerImg.info.pts = imgInfo.info.pts
      this.viewerImg.info.seqnum = imgInfo.info.seqnum
      this.viewerImg.info.userdata = imgInfo.info.userdata

      this.viewerImg.info.age = imgInfo.info.age
      this.viewerImg.info.gender = imgInfo.info.gender
      this.viewerImg.info.mask = imgInfo.info.mask
      this.viewerImg.info.info = imgInfo.info.info
      this.viewerImg.info.number = imgInfo.info.number
      this.viewerImg.info.color = imgInfo.info.color

      this.showViewer = true
    }
  }
}
</script>

<style lang="less" scoped>
.total_container {
  height: 100% !important;
}

.preview_outside_container {
  height: 100% !important;
}
.preview_main_container_normal {
  height: 100% !important;
}

.preview_main_container_normal_1cap {
  height: calc(100vh - 116px)!important;  // 42 + 50 + 14 + 10
}
.preview_main_container_normal_2cap {
  height: calc(100vh - 158px)!important;  // 84 + 50 + 14 + 10
}
.preview_main_container_ai {
  height: calc(100vh - 274px)!important;   // 210 + 50 + 14
}
.preview_main_container_ai_1cap {
  height: calc(100vh - 316px)!important;   // 210 + 50 + 14 + 42
}
.preview_main_container_ai_2cap {
  height: calc(100vh - 358px)!important;   // 210 + 50 + 14 + 84
}

.side_identify_events_container {
  padding: 0 !important;
  margin: 0 !important;
}



.preview_capture_left_container {
  width: 50% !important;
  height: 100% !important;
  padding: 0 !important;
}

.preview_container {
  height: 100% !important;
  width: 100%;

  .video_container {
    padding: 0 !important;
    height: 100%;
    width: 100%;
    background-color: #000;
    display: flex;
    flex-direction: column;
    align-content: center;
    align-items: center;

    .frtVideo {
      height: 100%;
      width: 100%;
      object-fit: fill !important;
      display: inline-block;
    }

    .frtVideo2 {
      height: 100%;
      width: 100%;
      object-fit: fill !important;
      display: inline-block;
    }
  }

  .video_controls_container {
    padding: 0 !important;
    background-color: #4c4c4c;
    display: flex;
    justify-content: space-between;
    align-items: center;

    .left_controls_container {
      display: flex;
      justify-content: left;
      color: #fff;

      .size_option {
        width: 60px;
        margin-left: 2px;
      }

      .src_option {
        width: 60px;
        margin-left: 10px;
      }

      .size_info {
        display: flex;
        align-items: center;
        margin-left: 20px;
        span {
          font-size: 12px!important;
        }
      }
    }

    .right_controls_container {
      display: flex;
      justify-content: left;
      flex-direction: row;
      align-content: center;
      align-items: stretch;
      .ezoom {
        width: 80px!important;
        height: 28px!important;
        margin-left: 10px!important;
        margin-right: 10px!important;
        .ezoom_slider {
          display: flex;
          align-items: center;
          width: 80px!important;
          height: 28px!important;
          .el-slider__button-wrapper{
            position: relative;
            top: -6px;
            width: 28px!important;
            height: 28px!important;
            &::after{
              content: '';
              width: 1px;
              height: 28px;
              position: absolute;
              top: -30px;
              left: 18px;
              border-right: 1px solid #96DD69;
            }
          }
        }
      }
    }
  }
}

.preview_seperator_line {
  height: 100% !important;
  width: 2px;
  border-right: solid #4c4c4c 1px;
}

.draw_button_enable {
  background-color: transparent !important;
  color: #fff;
  border: none;
  width: 28px;
  height: 28px;
  padding: 0;
  margin-left: 2px;

  /deep/ i {
    font-size: 20px;
  }
}

.draw_button_disable {
  background-color: transparent !important;
  color: #5c5c5c;
  border: none;
  width: 28px;
  height: 28px;
  padding: 0;
  margin-left: 2px;

  /deep/ i {
    font-size: 20px;
  }
}

.trigger_button {
  background-color: transparent !important;
  color: #fff;
  border: none;
  width: 28px;
  height: 28px;
  padding: 0;
  margin-left: 2px;

  /deep/ i {
    font-size: 26px;
  }
}

.full_button {
  background-color: transparent !important;
  color: #fff;
  border: none;
  width: 28px;
  height: 28px;
  padding: 0;
  margin-left: 2px;

  /deep/ i {
    font-size: 26px;
  }
}

.el-dropdown-link {
  color: #fff;
}

.el-dropdown-menu {
  width: 120px;
}

.snapshot_img_container {
  height: 84px !important;
  margin-left: 0px !important;
  padding-left: 0px !important;
  margin-right: 0px !important;
  padding-right: 0px !important;
}

.snapshot_img_container_half {
  height: 42px !important;
  margin-left: 0px !important;
  padding-left: 0px !important;
  margin-right: 0px !important;
  padding-right: 0px !important;
}

.hvcfp_img_container {
  height: 210px !important;
  margin-left: 0px !important;
  padding-left: 0px !important;
  margin-right: 0px !important;
  padding-right: 0px !important;
}

.fh_img_container {
  height: 210px !important;
  margin-left: 0px !important;
  padding-left: 0px !important;
  margin-right: 0px !important;
  padding-right: 0px !important;
}

.img_list_top {
  margin-top: 10px !important;
}

.img_list_bottom {
  margin-bottom: 0px !important;
}

.img_list {
  margin: 0px;
  padding: 0px;
}

.capture_img_container {
  height: 100%;
}

.happy_scroll_container {
  height: 100%;
}

.el-col {
  background: #4c4c4c;
}

.fp_img_col {
  height: 50px !important;
}

.hvcfp_img_col {
  height: 40px !important;
}

.image-small {
  height: 40px;
  width: 40px;
  position: relative;
  display: inline-block;
  overflow: hidden;
  justify-content: center;
  align-items: center;
  background: #4c4c4c;
}

.image-medium {
  height: 50px;
  width: 50px;
  position: relative;
  display: inline-block;
  overflow: hidden;
  justify-content: center;
  align-items: center;
  background: #4c4c4c;
}

.image-large {
  height: 160px;
  width: 160px;
  position: relative;
  display: inline-block;
  overflow: hidden;
  justify-content: center;
  align-items: center;
  background: #4c4c4c;
}

.image-slot-large {
  display: flex;
  justify-content: center;
  align-items: center;
  width: 100%;
  height: 100%;
  background: #4c4c4c;
  color: #909399;
}

.el-icon-picture-outline {
  position: absolute;
  left: 50%;
  top: 50%;
  transform: translate(-50%, -50%);
  color: gray;
  font-size: 30px;
}

.identify_events_container {
  width: 100% !important;
  height: 100% !important;
}

.identify_container {
  height: 160px !important;
  padding-top: 0 !important;
  padding-right: 0 !important;
  padding-left: 10px !important;
  white-space: nowrap;

  .image_info_container {
    background-color: #4c4c4c;
    height: 160px !important;
    display: flex;
    // justify-content:center;
    align-items: center;

    .image_col {
      height: 160px !important;
      width: 160px !important;
      margin: 0;
      padding: 0;
    }

    .info_container {
      padding-left: 10px;

      .info_text {
        color: #FFFFFF;
        font-size: 12px;
        margin-bottom: 10px;
      }
    }

  }
}

.view_container {
  background: #FFFFFF !important;
  height: 0px !important;
  padding-top: 0 !important;
  padding-right: 0 !important;
  padding-left: 10px !important;

  .dialog-title {
    font-size: 24px;
  }

  .view_image_info_container {
    background-color: #FFFFFF !important;
    height: 300px !important;

    .view_image_col {
      height: 300px !important;
      width: 300px !important;

      .image-view {
        height: 300px !important;
        width: 300px !important;
        background-color: #FFFFFF !important;
      }
    }

    .view_info_container {
      margin: 0;
      padding: 0;
      height: 100%;
      background-color: #FFFFFF !important;
      padding-left: 40px;

      .view_info_text {
        color: #000000;
        font-size: 18px;
        padding-bottom: 18px;
        background-color: #FFFFFF !important;
      }
    }
  }
}

.type_text {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 40px;
  width: 80px;
  background-color: #4c4c4c;
  color: #FFFFFF;
  font-size: 12px;
}

.type_text_fh {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 50px;
  width: 80px;
  background-color: #4c4c4c;
  color: #FFFFFF;
  font-size: 12px;
}

.events_container {
  width: 100% !important;
  height: 100% !important;
  padding-left: 10px !important;
  padding-top: 0 !important;
  padding-right: 0 !important;
  padding-bottom: 0 !important;
  margin-top: 0 !important;
  margin-right: 0 !important;
  margin-bottom: 0 !important;
}

.events_table {
  width: 100% !important;
  height: 100% !important;
  padding-top: 0 !important;
  padding-right: 0 !important;
  padding-bottom: 0 !important;
  margin-top: 0 !important;
  margin-right: 0 !important;
  margin-bottom: 0 !important;
  background-color: #4c4c4c !important;
  border: none;
}

/deep/ .el-table tr {
  background-color: transparent !important;
  color: #E0E0E0;
}

/deep/ .el-table--enable-row-transition .el-table__body td,
/deep/.el-table .cell {
  background-color: transparent;
  color: rgba(255, 255, 255, 0.7);
  border: none;
}

/deep/ .el-table__body tr.current-row>td {
  background-color: #5C5C5C;
  ;
}

.el-table::before {
  left: 0;
  bottom: 0;
  width: 100%;
  height: 0px;
}

/deep/ .el-table--enable-row-hover {
  .el-table__body tr:hover>td {
    background: #333333;
  }
}
</style>
