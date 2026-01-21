<template>
  <div>
    <div style="margin-bottom: 20px;">
      <el-form ref="formAiRef" :rules="formAiRules" :model="formAi" label-width="160px" label-position="left" size="mini">
        <el-form-item label="Camera:">
          <el-select v-model="formAi.src_id" @change="onChangeSrcID" :disabled="setting_sns_count == 1">
            <el-option v-for="item in Array.from({length:setting_sns_count},(item, index)=> index)" :key="item" :label="''+item" :value="item"></el-option>
          </el-select>
        </el-form-item>
      </el-form>
    </div>
    <el-tabs v-model="activeName" @tab-click="handleClick">
      <el-tab-pane label="AI Options" name="options">
        <el-form ref="formAiRef" :rules="formAiRules" :model="formAi" label-width="160px" label-position="left" size="mini">
          <el-form-item label="AI Enable:">
            <el-switch v-model="formAi.ai_attr.ai_enable"></el-switch>
          </el-form-item>
          <!-- <el-form-item label="Detect Mode:">
            <el-select v-model="formAi.ai_attr.detect_model" :disabled="true">
              <el-option v-for="item in detect_modes" :key="item.label" :label="formAi.ai_attr.detect_only ? item.label + '(no push image)' : item.label":value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="Detect FPS:">
            <el-select v-model="formAi.ai_attr.detect_fps">
              <el-option v-for="item in Array.from({length:15},(item, index)=> index+1)" :key="item" :label="''+item" :value="item"> </el-option>
            </el-select>
          </el-form-item-->
          <el-form-item label="Push Mode:">
            <el-select v-model="formAi.ai_attr.push_strategy.push_mode">
              <el-option v-for="item in push_strategy_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="" v-show="formAi.ai_attr.push_strategy.push_mode == 'FAST' || formAi.ai_attr.push_strategy.push_mode == 'INTERVAL'" >
            <el-form-item label="Interval(ms)">
              <el-select v-model="formAi.ai_attr.push_strategy.push_interval">
                <el-option v-for="item in push_interval_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
              </el-select>
            </el-form-item>
          </el-form-item>
          <el-form-item label="" v-show="formAi.ai_attr.push_strategy.push_mode == 'FAST' || formAi.ai_attr.push_strategy.push_mode == 'INTERVAL'" >
            <el-form-item label="Push Count">
              <el-select v-model="formAi.ai_attr.push_strategy.push_count">
                <el-option v-for="item in Array.from({length:5},(item, index)=> index+1)" :key="item" :label="''+item" :value="item"> </el-option>
              </el-select>
            </el-form-item>
          </el-form-item>
          <!-- el-form-item label="Push Same Frame:">
            <el-switch v-model="formAi.ai_attr.push_strategy.push_same_frame"></el-switch>
          </el-form-item> -->
        </el-form>
      </el-tab-pane>
      <el-tab-pane label="Event Report" name="events">
        <el-form ref="formAiRef" :rules="formAiRules" :model="formAi" label-width="160px" label-position="left" size="mini">
          <el-form-item label="Motion Detect:">
            <el-switch v-model="formAi.ai_attr.events.motion_detect.enable"></el-switch>
            <el-form-item label="Y Threshold(1-255)" v-show="formAi.ai_attr.events.motion_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.motion_detect.threshold_y" :min="1" :max="255" show-input></el-slider>
            </el-form-item>
            <el-form-item label="Confidence(1-100)" v-show="formAi.ai_attr.events.motion_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.motion_detect.confidence" :min="1" :max="100" show-input></el-slider>
            </el-form-item>
          </el-form-item>
          <el-form-item label="Occlusion Detect:">
            <el-switch v-model="formAi.ai_attr.events.occlusion_detect.enable"></el-switch>
            <el-form-item label="Y Threshold(1-255):" v-show="formAi.ai_attr.events.occlusion_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.occlusion_detect.threshold_y" :min="1" :max="255" show-input></el-slider>
            </el-form-item>
            <el-form-item label="Confidence(1-100):" v-show="formAi.ai_attr.events.occlusion_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.occlusion_detect.confidence" :min="1" :max="100" show-input></el-slider>
            </el-form-item>
          </el-form-item>
          <el-form-item label="Scene Change Detect:">
            <el-switch v-model="formAi.ai_attr.events.scene_change_detect.enable"></el-switch>
            <el-form-item label="Threshold(1-100):" v-show="formAi.ai_attr.events.scene_change_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.scene_change_detect.threshold_y" :min="1" :max="100" show-input></el-slider>
            </el-form-item>
            <el-form-item label="Confidence(1-100):" v-show="formAi.ai_attr.events.scene_change_detect.enable">
              <el-slider v-model="formAi.ai_attr.events.scene_change_detect.confidence" :min="1" :max="100" show-input></el-slider>
            </el-form-item>
          </el-form-item>
        </el-form>
      </el-tab-pane>
    </el-tabs>
    <div style="margin-bottom: 20px;">
      <el-divider></el-divider>
      <el-button type="primary" @click="onSubmit" size="mini">OK</el-button>
    </div>
  </div>
</template>

<script>
export default {
  props:["setting_sns_count"],
  data () {
    return {
      activeName: 'options',
      detect_modes: [
        {
          label: 'facehuman',
          value: 'facehuman'
        },
        {
          label: 'hvcfp',
          value: 'hvcfp'
        }
      ],
      push_strategy_options: [
        {
          label: 'FAST',
          value: 'FAST'
        },
        {
          label: 'INTERVAL',
          value: 'INTERVAL'
        },
        {
          label: 'BEST FRAME',
          value: 'BEST_FRAME'
        }
      ],
      push_interval_options: [
        {
          label: '1000',
          value: 1000
        },
        {
          label: '2000',
          value: 2000
        },
        {
          label: '3000',
          value: 3000
        },
        {
          label: '5000',
          value: 5000
        },
        {
          label: '10000',
          value: 10000
        },
        {
          label: '20000',
          value: 20000
        }
      ],
      formAi: {
        src_id: 0,
        ai_attr: {
          ai_enable: true,
          detect_model: 'hvcfp',
          detect_fps:15,
          push_strategy: {
            push_mode: 'BEST_FRAME',
            push_interval: 2000,
            push_count: 1,
            push_same_frame: true
          },
          detect_only: false,
          facehuman: {
            face_detect: {
              enable: true,
              draw_rect: true
            },
            body_detect: {
              enable: true,
              draw_rect: true
            },
            face_identify: {
              enable: true
            }
          },
          hvcfp: {
            face_detect: {
              enable: true,
              draw_rect: true
            },
            body_detect: {
              enable: true,
              draw_rect: true
            },
            vechicle_detect: {
              enable: true,
              draw_rect: true
            },
            cycle_detect: {
              enable: true,
              draw_rect: true
            },
            plate_detect: {
              enable: true,
              draw_rect: true
            },
            plate_identify: {
              enable: true
            }
          },
          events: {
            motion_detect: {
              enable: true,
              threshold_y: 50,
              confidence: 80
            },
            occlusion_detect: {
              enable: true,
              threshold_y: 100,
              confidence: 80
            },
            scene_change_detect: {
              enable: false,
              threshold_y: 60,
              confidence: 60
            }
          }
        }
      },
      formAiRules: {
        // pending
      }
    }
  },
  created () {
    console.log('ai++')
    this.getInfo()
  },
  methods: {
    handleClick(tab, event) {
      // console.log(tab, event);
    },
    onSubmit () {
      this.$refs.formAiRef.validate(async valid => {
        if (!valid) return false
        try {
          const { data: res } = await this.$http.post('setting/ai', this.formAi)
          console.log('ai post return: ', res)
          if (res.meta.status === 200) {
            this.$message.success('Modification successful')
          } else {
            this.$message.error('Modification failed')
          }
        } catch (error) {
          this.$message.error('Modification failed')
        }
      })
    },
    async getInfo () {
      try {
        const { data: res } = await this.$http.get('setting/ai', {params:{src_id: this.formAi.src_id}})
        console.log('ai get return: ', res)
        if (res.meta.status === 200) {
          this.formAi.ai_attr.ai_enable = res.data.ai_attr.ai_enable
          this.formAi.ai_attr.detect_model = res.data.ai_attr.detect_model
          this.formAi.ai_attr.detect_fps = res.data.ai_attr.detect_fps
          this.formAi.ai_attr.push_strategy.push_mode = res.data.ai_attr.push_strategy.push_mode
          this.formAi.ai_attr.push_strategy.push_interval = res.data.ai_attr.push_strategy.push_interval
          this.formAi.ai_attr.push_strategy.push_count = res.data.ai_attr.push_strategy.push_count
          this.formAi.ai_attr.push_strategy.push_same_frame = res.data.ai_attr.push_strategy.push_same_frame

          this.formAi.ai_attr.detect_only = res.data.ai_attr.detect_only
          if (this.formAi.ai_attr.detect_model === 'facehuman') {
            this.formAi.ai_attr.facehuman.face_detect.enable = res.data.ai_attr.facehuman.face_detect.enable
            this.formAi.ai_attr.facehuman.face_detect.draw_rect = res.data.ai_attr.facehuman.face_detect.draw_rect
            this.formAi.ai_attr.facehuman.body_detect.enable = res.data.ai_attr.facehuman.body_detect.enable
            this.formAi.ai_attr.facehuman.body_detect.draw_rect = res.data.ai_attr.facehuman.body_detect.draw_rect
            this.formAi.ai_attr.facehuman.face_identify.enable = res.data.ai_attr.facehuman.face_identify.enable
          } else {
            this.formAi.ai_attr.hvcfp.face_detect.enable = res.data.ai_attr.hvcfp.face_detect.enable
            this.formAi.ai_attr.hvcfp.face_detect.draw_rect = res.data.ai_attr.hvcfp.face_detect.draw_rect
            this.formAi.ai_attr.hvcfp.body_detect.enable = res.data.ai_attr.hvcfp.body_detect.enable
            this.formAi.ai_attr.hvcfp.body_detect.draw_rect = res.data.ai_attr.hvcfp.body_detect.draw_rect
            this.formAi.ai_attr.hvcfp.vechicle_detect.enable = res.data.ai_attr.hvcfp.vechicle_detect.enable
            this.formAi.ai_attr.hvcfp.vechicle_detect.draw_rect = res.data.ai_attr.hvcfp.vechicle_detect.draw_rect
            this.formAi.ai_attr.hvcfp.cycle_detect.enable = res.data.ai_attr.hvcfp.cycle_detect.enable
            this.formAi.ai_attr.hvcfp.cycle_detect.draw_rect = res.data.ai_attr.hvcfp.cycle_detect.draw_rect
            this.formAi.ai_attr.hvcfp.plate_detect.enable = res.data.ai_attr.hvcfp.plate_detect.enable
            this.formAi.ai_attr.hvcfp.plate_detect.draw_rect = res.data.ai_attr.hvcfp.plate_detect.draw_rect
            this.formAi.ai_attr.hvcfp.plate_identify.enable = res.data.ai_attr.hvcfp.plate_identify.enable
          }

          this.formAi.ai_attr.events.motion_detect.enable = res.data.ai_attr.events.motion_detect.enable
          this.formAi.ai_attr.events.motion_detect.threshold_y = res.data.ai_attr.events.motion_detect.threshold_y
          this.formAi.ai_attr.events.motion_detect.confidence = res.data.ai_attr.events.motion_detect.confidence
          this.formAi.ai_attr.events.occlusion_detect.enable = res.data.ai_attr.events.occlusion_detect.enable
          this.formAi.ai_attr.events.occlusion_detect.threshold_y = res.data.ai_attr.events.occlusion_detect.threshold_y
          this.formAi.ai_attr.events.occlusion_detect.confidence = res.data.ai_attr.events.occlusion_detect.confidence
          this.formAi.ai_attr.events.scene_change_detect.enable = res.data.ai_attr.events.scene_change_detect.enable
          this.formAi.ai_attr.events.scene_change_detect.threshold_y = res.data.ai_attr.events.scene_change_detect.threshold_y
          this.formAi.ai_attr.events.scene_change_detect.confidence = res.data.ai_attr.events.scene_change_detect.confidence
        }
      } catch (error) {
        this.$message.error('Failed to get information')
      }
    },
    onChangeSrcID() {
      this.getInfo()
    }
  }
}
</script>

<style lang="less" scoped>
.el-form {
  width: 700px!important;
}
.el-input {
  width: 100%!important;
}
.el-select {
  width: 140px!important;
}
.el-option {
  width: 140px!important;
}
</style>
