<template>
  <div>
    <el-form ref="formCameraRef" :rules="formCameraRules" :model="formCamera" label-width="160px" size="mini">
      <el-form-item label="Camera:">
        <el-select v-model="formCamera.src_id" @change="onChangeSrcID" :disabled="setting_sns_count == 1 || formCamera.camera_attr.disable_camera_select">
          <el-option v-for="item in Array.from({ length: setting_sns_count }, (item, index) => index)" :key="item" :label="'' + item"
            :value="item"></el-option>
        </el-select>
      </el-form-item>
      <el-form-item label="Capture:" v-show="formCamera.camera_attr.auto_capture_enable">
        <el-switch v-model="formCamera.camera_attr.auto_capture"></el-switch>
      </el-form-item>
      <el-form-item label="Work Mode:" v-show="formCamera.camera_attr.switch_work_mode_enable">
        <el-select v-model="formCamera.camera_attr.sns_work_mode" @change="onChangeSnsMode">
          <el-option v-for="item in sns_mode_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
        </el-select>
      </el-form-item>
      <el-form-item label="Camera FPS:" v-show="formCamera.camera_attr.switch_PN_mode_enable">
         <el-select v-model="formCamera.camera_attr.framerate">
          <el-option v-for="item in framerate_options[formCamera.camera_attr.sns_work_mode - 1]" :key="item.label" :label="item.label" :value="item.value">
         </el-option>
        </el-select>
      </el-form-item>
      <el-form-item label="Day/Night Mode:">
        <el-select v-model="formCamera.camera_attr.daynight">
          <el-option v-for="item in daynight_options" :key="item.label" :label="item.label" :value="item.value">
          </el-option>
        </el-select>
      </el-form-item>
      <el-form-item label="Enhanced Algorithm Pipe:" v-show="formCamera.camera_attr.switch_dump_raw_pipe_enable">
          <el-select v-model="formCamera.camera_attr.dump_raw_pipe">
            <el-option v-for="item in dump_raw_pipe_opts" :key="item.label" :label="item.label" :value="item.value">
            </el-option>
          </el-select>
      </el-form-item>
      <el-form-item label="Rotation:" v-show="formCamera.camera_attr.switch_rotation_enable">
        <el-select v-model="formCamera.camera_attr.rotation">
          <el-option v-for="item in rotation_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
        </el-select>
      </el-form-item>
      <el-form label-width="200px">
        <el-row>
          <el-col :span="2">
            <el-form-item label="Mirror:" v-show="formCamera.camera_attr.switch_mirror_flip_enable">
              <el-switch v-model="formCamera.camera_attr.mirror"></el-switch>
            </el-form-item>
          </el-col>
          <el-col :span="2">
            <el-form-item label="Flip:" v-show="formCamera.camera_attr.switch_mirror_flip_enable">
              <el-switch v-model="formCamera.camera_attr.flip"></el-switch>
            </el-form-item>
          </el-col>
        </el-row>
      </el-form>
      <el-form-item label="Shutter:" v-show="formCamera.camera_attr.shutter_option_enable">
        <el-select v-model="formCamera.camera_attr.shutter_type">
          <el-option v-for="item in shutter_options" :key="item.label" :label="item.label" :value="item.value">
          </el-option>
        </el-select>
      </el-form-item>

      <!-- el-form-item label="NR:">
        <el-switch v-model="formCamera.camera_attr.nr_mode"></el-switch>
      </el-form-item -->
      <el-form-item label="EIS:" v-show="formCamera.camera_attr.eis_support">
        <el-switch v-model="formCamera.camera_attr.eis"></el-switch>
      </el-form-item>

      <el-form-item label="Enable Sub Channel:" v-show="formCamera.camera_attr.switch_vsubchn_enable">
        <el-switch v-model="formCamera.camera_attr.vsubchn_on"></el-switch>
      </el-form-item>

      <el-form-item label="Flash" v-show="formCamera.flash_attr_enable">
      </el-form-item>
      <el-form-item label="" v-show="formCamera.flash_attr_enable">
        <el-table :data="formCamera.flash_attr" style="width 100%">
          <el-table-column prop="lane_id" label="Lane ID" width="60">
          </el-table-column>

          <el-table-column prop="flash_id" label="Flash ID" width="200">
          <template slot-scope="scope">
           <el-select v-model="scope.row.flash_id" style="width:91%" size="mini" >
                <div id="flash_idx">
                  <el-option v-for="item in flash_option" :key="item.label" :label="item.label" :value="item.value"> </el-option>
                </div>
            </el-select>
            </template>
          </el-table-column>

          <el-table-column prop="trigger_count" label="Trigger Count" width="200">
           <template slot-scope="scope">
              <el-input-number controls-position="right" :min="-1" :max="9" :step="1" v-model="scope.row.trigger_count" style="width:91%" size="mini"></el-input-number>
           </template>
          </el-table-column>

          <el-table-column prop="frame_interval" label="Trigger Interval" width="200">
            <template slot-scope="scope">
              <el-input-number controls-position="right" :min="-1" :max="9" :step="1" v-model="scope.row.frame_interval" style="width:91%" size="mini"></el-input-number>
            </template>
          </el-table-column>
        </el-table>
      </el-form-item>

      <el-form-item>
        <el-divider></el-divider>
        <el-button type="primary" @click="onSubmit">OK</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
export default {
  props: ["setting_sns_count"],
  data() {
    return {
      flash_option: [
        {
          lable: '-1',
          value: -1
        },
        {
          label: '0',
          value: 0
        },
        {
          lable: '1',
          value: 1
        },
        {
          label: '2',
          value: 2
        },
        {
          label: '3',
          value: 3
        }
      ],
      sns_mode_options: [
        {
          label: 'SDR',
          value: 1
        },
        {
          label: 'HDR',
          value: 2
        }
      ],
      rotation_options: [
        {
          label: '0°',
          value: 0
        },
        {
          label: '90°',
          value: 1
        },
        {
          label: '180°',
          value: 2
        },
        {
          label: '270°',
          value: 3
        }
      ],
      daynight_options: [
        {
          label: 'Day Mode',
          value: 0
        },
        {
          label: 'Night Mode',
          value: 1
        }
      ],
      isp_mode: [
        {
          label: 'Auto Mode',
          value: 1
        },
        {
          label: 'Manual Mode',
          value: 0
        }
      ],
      framerate_options: [
        [
          {
            label: '25',
            value: 25
          },
          {
            label: '30',
            value: 30
          }
        ],
        [
          {
            label: '25',
            value: 25
          },
          {
            label: '30',
            value: 30
          }
        ]
      ],
      shutter_options: [
        {
          label: 'Single Shutter',
          value: 0
        },
        {
          label: 'Double Shutter',
          value: 1
        },
        {
          label: 'Triple Shutter',
          value: 2
        }
      ],
      dump_raw_pipe_opts: [
        {
          label: 'Pipe0',
          value: 0
        },
        {
          label: 'Pipe1',
          value: 1
        },
        {
          label: 'Pipe2',
          value: 2
        }
      ],
      formCamera: {
        src_id: 0,
        camera_attr: {
          sns_work_mode: 1,
          rotation: 0,
          mirror: false,
          flip: false,
          framerate: 25,
          daynight: 0,
          dump_raw_pipe: 2,
          nr_mode: true,
          eis_support: false,
          eis: false,
          vsubchn_on: true,
          auto_capture: true,
          auto_capture_enable: true,
          switch_work_mode_enable: false,
          switch_PN_mode_enable: false,
          shutter_type: 0,
          shutter_option_enable: false,
          switch_dump_raw_pipe_enable: false,
          switch_vsubchn_enable: false,
          framerate_opts: [
            [25,30],
            [25,30]
          ],
          disable_camera_select: false,
        },
        flash_attr_enable: false,
        flash_attr: [
        {
          lane_id: '1',
          flash_id: '2',
          trigger_count: '1',
          frame_interval: '3'
        }, {
          lane_id: '2',
          flash_id: '-1',
          trigger_count: '1',
          frame_interval: '3'
        }, {
          lane_id: '3',
          flash_id: '-1',
          trigger_count: '1',
          frame_interval: '3'
        }, {
          lane_id: '4',
          flash_id: '-1',
          trigger_count: '1',
          frame_interval: '3'
        }
        ]
      },
      formCameraRules: {
        // pending
      }
    }
  },
  created() {
    console.log('camera++')
    this.getInfo()
  },
  methods: {
    onSubmit() {
      this.$refs.formCameraRef.validate(async valid => {
        if (!valid) return false
        try {
          const { data: res } = await this.$http.post('setting/camera', this.formCamera)
          console.log('camera get return: ', res)
          if (res.meta.status === 200) {
            this.$message.success('Modification successful')
          } else {
            this.$message.error('Modification failed')
          }
        } catch (error) {
          this.$message.error('Modification failed')
        }
        this.getInfo()
      })
    },
    async getInfo() {
      try {
        const { data: res } = await this.$http.get('setting/camera', { params: { src_id: this.formCamera.src_id } })
        console.log('camera get return: ', res)
        if (res.meta.status === 200) {
          this.formCamera.camera_attr.sns_work_mode = res.data.camera_attr.sns_work_mode
          this.formCamera.camera_attr.rotation = res.data.camera_attr.rotation
          this.formCamera.camera_attr.mirror = res.data.camera_attr.mirror
          this.formCamera.camera_attr.flip = res.data.camera_attr.flip
          this.formCamera.camera_attr.daynight = res.data.camera_attr.daynight
          this.formCamera.camera_attr.framerate = res.data.camera_attr.framerate
          this.formCamera.camera_attr.nr_mode = res.data.camera_attr.nr_mode
          this.formCamera.camera_attr.dump_raw_pipe = res.data.camera_attr.dump_raw_pipe
          this.formCamera.camera_attr.eis_support = res.data.camera_attr.eis_support
          this.formCamera.camera_attr.eis = res.data.camera_attr.eis
          this.formCamera.camera_attr.vsubchn_on = res.data.camera_attr.vsubchn_on
          this.formCamera.camera_attr.auto_capture = res.data.camera_attr.auto_capture
          this.formCamera.camera_attr.auto_capture_enable = res.data.camera_attr.auto_capture_enable
          this.formCamera.camera_attr.switch_work_mode_enable = res.data.camera_attr.switch_work_mode_enable
          this.formCamera.camera_attr.switch_PN_mode_enable = res.data.camera_attr.switch_PN_mode_enable
          this.formCamera.camera_attr.switch_mirror_flip_enable = res.data.camera_attr.switch_mirror_flip_enable
          this.formCamera.camera_attr.switch_rotation_enable = res.data.camera_attr.switch_rotation_enable
          this.formCamera.camera_attr.disable_camera_select = res.data.camera_attr.disable_camera_select
          this.formCamera.camera_attr.shutter_type = res.data.camera_attr.shutter_type
          this.formCamera.camera_attr.shutter_option_enable = res.data.camera_attr.shutter_option_enable
          this.formCamera.flash_attr_enable = res.data.flash_attr_enable
          this.formCamera.camera_attr.switch_dump_raw_pipe_enable = res.data.camera_attr.switch_dump_raw_pipe_enable
          this.formCamera.camera_attr.switch_vsubchn_enable = res.data.camera_attr.switch_vsubchn_enable

          if (res.data.framerate_opts) {
            this.formCamera.framerate_opts = res.data.framerate_opts
            this.framerate_options = []
                    for (let i = 0; i < this.formCamera.framerate_opts.length; i++) {
                      this.framerate_options.push([])
                      for (let j = 0; j < this.formCamera.framerate_opts[i].length; j++) {
                        this.framerate_options[i].push({ label: '' + this.formCamera.framerate_opts[i][j], value: this.formCamera.framerate_opts[i][j] })
                      }
                    }
          }


          if (this.formCamera.flash_attr_enable) {
            for (let i = 0; i < res.data.flash_attr.length; i++) {
              this.formCamera.flash_attr[i].flash_id = res.data.flash_attr[i].flash_id;
              this.formCamera.flash_attr[i].frame_interval = res.data.flash_attr[i].frame_interval;
              this.formCamera.flash_attr[i].trigger_count = res.data.flash_attr[i].trigger_count;
            }
          }
        }
      } catch (error) {
        this.$message.error('Failed to get information')
      }
    },
    onChangeSrcID() {
      this.getInfo()
    },
    onChangeSnsMode(mode) {
      if (this.formCamera.camera_attr.switch_PN_mode_enable) {
        this.formCamera.camera_attr.framerate = this.framerate_options[this.formCamera.camera_attr.sns_work_mode - 1][0].value
      }
    },
  }
}
</script>

<style lang="less" scoped>
.el-input {
  width: 200px !important;
}

.el-select {
  width: 200px;
}

.el-slider {
  width: 400px;
}

.el-form-item {
  margin-bottom: 10px;
}
</style>
