<template>
  <div class="storage-container">
    <el-form ref="formStorageRef" :rules="formStorageRules" :model="formStorage" label-position="left" label-width="180px">
      <el-form-item label="Record Storage Strategy In Device">
      </el-form-item>
      <el-form-item label="Enable Record" class="storage-item">
        <el-switch v-model="formStorage.storage.rec_actived"></el-switch>
      </el-form-item>
      <el-form-item label="Enable Loop Record" class="storage-item">
        <el-switch v-model="formStorage.storage.rec_loop_cover" :disabled="!formStorage.storage.rec_actived"></el-switch>
      </el-form-item>
      <el-form-item label="Total Record Storage Space(MB)" class="storage-item">
        <el-col :span="8">
          <el-input v-model="formStorage.storage.rec_total" :readonly="true" :disabled="!formStorage.storage.rec_actived"></el-input>
        </el-col>
      </el-form-item>
      <el-form-item label="Maximum Size of One Record File(MB)" class="storage-item" prop="storage.rec_threshold">
        <el-input-number v-model="formStorage.storage.rec_threshold" :min="16" :max="1024" :step="64" :disabled="!formStorage.storage.rec_actived" controls-position="right" placeholder="File Size threshold"></el-input-number>
      </el-form-item>
      <el-form-item label="Maximum Number of Record Files" class="storage-item" prop="storage.rec_max_count">
        <el-input-number v-model="formStorage.storage.rec_max_count" :min="1" :max="100" :step="10" :disabled="!formStorage.storage.rec_actived" controls-position="right" placeholder="Maximum number of files"></el-input-number>
      </el-form-item>
      <el-form-item label="Storage Path" class="storage-item" prop="storage.rec_path">
        <el-col :span="8">
          <el-input v-model="formStorage.storage.rec_path" :readonly="true" placeholder="Storage Path In Device"></el-input>
        </el-col>
      </el-form-item>
      <p style="color:indianred;margin-left: 60px;">Note: Single video file threshold (MB) x video file number limit (pieces), must be less than the total video file space (MB)</p>
      <el-form-item label="Record Storage Strategy In Web">
      </el-form-item>
      <el-form-item label="Maximum Size of One Record File(MB)" class="storage-item" prop="rec_threshold_web">
        <el-input-number v-model="formStorage.rec_threshold_web" :min="16" :max="1024" :step="64" controls-position="right" placeholder="File Size threshold"></el-input-number>
      </el-form-item>
      <p style="color:indianred;margin-left: 60px;">Note: Video playback only involves web video. When downloading video, it is fixed to the Downloads directory of the current user.</p>
      <el-form-item>
        <el-button type="primary" @click="onSubmit">OK</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>

<script>
export default {
  data () {
    const validate_rec_path = (rule, value, callback) => {
      if (value.length > 64) {
        callback(new Error())
      }
      else {
        var p = /^\/(\w+\/?)+$/;
        if(!p.test(value))
        {
          callback(new Error())
        }
      }
      callback()
    }
    return {
      formStorage: {
        storage: {
          rec_actived: true,
          rec_loop_cover: true,
          rec_total: 2560,
          rec_threshold: 256,
          rec_max_count: 10,
          rec_path: '/mnt/sdcard/'
        },
        rec_threshold_web: 1024,
      },
      formStorageRules: {
        'storage.rec_threshold': [{ type: 'integer', required: true, message: 'Please enter the maximum size of the video file in device', trigger: 'blur' }],
        'storage.rec_max_count': [{ type: 'integer', required: true, message: 'Please enter the maximum number of video files in device', trigger: 'blur' }],
        'storage.rec_path': [{ type: 'string', required: true, message: 'Please enter storage path in device', trigger: 'blur' },
                             { required: true, validator: validate_rec_path, message: "The length of storage path in device exceeds 64 bits, or the path is invalid (Linux directory format is required, such as:\'/mnt/sdcard/\')", trigger: "blur"}],
        'rec_threshold_web': [{ type: 'integer', required: true, message: 'Please enter the maximum size of the video file in web', trigger: 'blur' }]
      }
    }
  },
  created () {
    console.log('storage++')
    this.getInfo()
  },
  methods: {
    async onSubmit () {
      try {
        let _fileSize = this.formStorage.storage.rec_threshold
        let _fileNum  = this.formStorage.storage.rec_max_count
        if (_fileSize * _fileNum > this.formStorage.storage.rec_total) {
          this.$message.error('The single video file threshold x the upper limit of the number of video files exceeds the total video space on the board!')
          return
        }

        const { data: res } = await this.$http.post('setting/storage', this.formStorage)
        console.log('storage set return: ', res)
        if (res.meta.status === 200) {
          this.$message.success('Modification successful')
        } else if (res.meta.status === 404) {
          this.$message.error('Failed to create the storage path in device')
        } else {
          this.$message.error('Modification failed')
        }
        window.localStorage.setItem('rec_threshold_web', this.formStorage.rec_threshold_web);
      } catch (error) {
        this.$message.error('Modification failed')
      }
    },
    async getInfo () {
      try {
        const { data: res } = await this.$http.get('setting/storage')
        console.log('storage get return: ', res)
        if (res.meta.status === 200) {
          this.formStorage.storage = res.data.storage
          if (this.formStorage.storage.rec_total === 0) {
            this.formStorage.storage.rec_total = 2560
          }
        }
        var _rec_threshold_web = window.localStorage.getItem('rec_threshold_web')
        if (_rec_threshold_web != undefined) {
          this.formStorage.rec_threshold_web = _rec_threshold_web
        }
      } catch (error) {
        this.$message.error('Failed to get information')
      }
    }
  }
}
</script>

<style lang="less" scoped>
.storage-container {
  height: 100%;
}
.storage-item {
  margin-left: 60px;
}
.el-input-number {
  width: 120px;
}
.el-select {
  width: 100px;
}
.inline {
  display: inline-block;
 }
</style>
