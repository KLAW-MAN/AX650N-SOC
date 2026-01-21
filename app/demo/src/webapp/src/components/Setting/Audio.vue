<template>
  <div>
    <el-form ref="formAudioRef" :rules="formAudioRules" :model="formAudio" label-width="160px" size="mini">
      <el-form-item label="Microphone Parameters:"/>
      <el-form-item label="Volume:" class="inline">
        <el-slider v-model="formAudio.capture_attr.volume_val" :min="0" :max="10" :show-tooltip="false" show-input
          @change="onChangeVolume"></el-slider>
      </el-form-item>
      <el-form-item label="Speaker Parameters:"/>
      <el-form-item label="Volume:" class="inline">
        <el-slider v-model="formAudio.play_attr.volume_val" :min="0" :max="10" :show-tooltip="false" show-input
          @change="onChangeVolume"></el-slider>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
export default {
  props: ["setting_sns_count"],
  data() {
    return {
      formAudio: {
        src_id: 0,
        capture_attr: {
          volume_val: 1,
        },
        play_attr: {
          volume_val: 1,
        }

      },
      formAudioRules: {
        // pending
      }
    }
  },
  created() {
    console.log('Audio++')
    this.getInfo()
  },
  methods: {
    onSubmit() {
      this.$refs.formAudioRef.validate(async valid => {
        if (!valid) return false
        try {
          const { data: res } = await this.$http.post('setting/audio', this.formAudio)
          console.log('Audio get return: ', res)
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
        const { data: res } = await this.$http.get('setting/audio', { params: { src_id: this.formAudio.src_id } })
        console.log('Audio get return: ', res)
        if (res.meta.status === 200) {
          this.formAudio.capture_attr.volume_val = res.data.info.capture_attr.volume_val
          this.formAudio.play_attr.volume_val = res.data.info.play_attr.volume_val
        }
      } catch (error) {
        this.$message.error('Failed to get information')
      }
    },
    onChangeSrcID() {
      this.getInfo()
    },
    onChangeVolume() {
      this.$refs.formAudioRef.validate(async valid => {
        if (!valid) return false
        try {
          const { data: res } = await this.$http.post('setting/audio', this.formAudio)
          console.log('Audio get return: ', res)
          if (res.meta.status === 200) {
            this.$message.success('Modification successful')
          } else {
            this.$message.error('Modification failed')
          }
        } catch (error) {
          this.$message.error('Modification failed')
        }
      })
    }
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
</style>
