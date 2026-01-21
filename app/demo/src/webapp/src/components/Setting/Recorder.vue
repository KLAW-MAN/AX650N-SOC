<template>
  <div>
    <el-form ref="formRecordRef" :rules="formRecordRules" :model="formRecord" label-width="100px" size="mini">
      <el-form-item label="Camera:">
        <el-select v-model="formRecord.src_id" @change="onChangeSrcID" :disabled="setting_sns_count == 1">
          <el-option v-for="item in Array.from({ length: setting_sns_count }, (item, index) => index)" :key="item" :label="'' + item"
            :value="item"></el-option>
        </el-select>
      </el-form-item>
      <el-form-item label="Store path:">
        <el-input v-model="formRecord.store_path" readonly></el-input>
      </el-form-item>
      <el-form-item label="Main Stream0 Record:">
        <el-switch v-model="formRecord.chn_attr[0].enable" :disabled="!formRecord.chn_attr[0].support"></el-switch>
      </el-form-item>
      <el-form-item label="" label-width="90px">
          <el-form-item label="Record Type:" class="inline" v-show="formRecord.chn_attr[0].support && formRecord.chn_attr[0].enable">
            <el-select v-model="formRecord.chn_attr[0].record_type">
              <el-option v-for="item in record_type_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="Raw Stream Format:" class="inline" v-show="formRecord.chn_attr[0].support && formRecord.chn_attr[0].enable">
            <el-input v-model="raw_stream_format[formRecord.chn_attr[0].raw_stream]" readonly></el-input>
          </el-form-item>
      </el-form-item>
      <el-form-item label="Sub Stream1 Record:" v-show="formRecord.chn_num > 1">
        <el-switch v-model="formRecord.chn_attr[1].enable" :disabled="!formRecord.chn_attr[1].support"></el-switch>
      </el-form-item>
      <el-form-item label="" label-width="90px">
          <el-form-item label="Record Type:" class="inline" v-show="formRecord.chn_attr[1].support && formRecord.chn_attr[1].enable">
            <el-select v-model="formRecord.chn_attr[1].record_type">
              <el-option v-for="item in record_type_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="Raw Stream Format:" class="inline" v-show="formRecord.chn_attr[1].support && formRecord.chn_attr[1].enable">
            <el-input v-model="raw_stream_format[formRecord.chn_attr[1].raw_stream]" readonly></el-input>
          </el-form-item>
      </el-form-item>
      <el-form-item label="Sub Stream2 Record::" v-show="formRecord.chn_num > 2">
        <el-switch v-model="formRecord.chn_attr[2].enable" :disabled="!formRecord.chn_attr[2].support"></el-switch>
      </el-form-item>
      <el-form-item label="" label-width="90px">
          <el-form-item label="Record Type:" class="inline" v-show="formRecord.chn_attr[2].support && formRecord.chn_attr[2].enable">
            <el-select v-model="formRecord.chn_attr[2].record_type">
              <el-option v-for="item in record_type_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="Raw Stream Format:" class="inline" v-show="formRecord.chn_attr[2].support && formRecord.chn_attr[2].enable">
            <el-input v-model="raw_stream_format[formRecord.chn_attr[2].raw_stream]" readonly></el-input>
          </el-form-item>
      </el-form-item>
      <el-form-item label="Sub Stream3 Record::" v-show="formRecord.chn_num > 3">
        <el-switch v-model="formRecord.chn_attr[3].enable" :disabled="!formRecord.chn_attr[3].support"></el-switch>
      </el-form-item>
      <el-form-item label="" label-width="90px">
          <el-form-item label="Record Type:" class="inline" v-show="formRecord.chn_attr[3].support && formRecord.chn_attr[3].enable">
            <el-select v-model="formRecord.chn_attr[3].record_type">
              <el-option v-for="item in record_type_options" :key="item.label" :label="item.label" :value="item.value"> </el-option>
            </el-select>
          </el-form-item>
          <el-form-item label="Raw Stream Format:" class="inline" v-show="formRecord.chn_attr[3].support && formRecord.chn_attr[3].enable">
            <el-input v-model="raw_stream_format[formRecord.chn_attr[3].raw_stream]" readonly></el-input>
          </el-form-item>
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
      record_type_options: [
        {
          label: 'MP4',
          value: 0
        },
        {
          label: 'RAW',
          value: 1
        }
      ],
      raw_stream_format: ['H264', 'H265', 'MJPG'],
      formRecord: {
        src_id: 0,
        chn_num: 3,
        store_path: './',
        chn_attr: [
          {
            support: true,
            enable: false,
            record_type: 0,
            raw_stream: 0
          },
          {
            support: true,
            enable: false,
            record_type: 0,
            raw_stream: 0
          },
          {
            support: true,
            enable: false,
            record_type: 0,
            raw_stream: 0
          },
          {
            support: true,
            enable: false,
            record_type: 0,
            raw_stream: 0
          }
        ]
      }
    }
  },
  created() {
    console.log('record ++')
    this.getInfo()
  },
  methods: {
    onSubmit() {
      this.$refs.formRecordRef.validate(async valid => {
        if (!valid) return false
        try {
          const { data: res } = await this.$http.post('setting/record', this.formRecord)
          console.log('record get return: ', res)
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
        const { data: res } = await this.$http.get('setting/record', { params: { src_id: this.formRecord.src_id } })
        console.log('record get return: ', res)
        if (res.meta.status === 200) {
          this.formRecord.chn_num = (res.data.record_attr.chn_num > 4) ? 4 : res.data.record_attr.chn_num
          this.formRecord.store_path = res.data.record_attr.store_path
          for (let i = 0; i < this.formRecord.chn_num; i++) {
            this.formRecord.chn_attr[i].support = res.data.record_attr.chn_attr[i].support;
            this.formRecord.chn_attr[i].enable = res.data.record_attr.chn_attr[i].enable;
            this.formRecord.chn_attr[i].record_type = res.data.record_attr.chn_attr[i].record_type;
            this.formRecord.chn_attr[i].raw_stream = res.data.record_attr.chn_attr[i].raw_stream;
          }
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
.el-input {
  width: 200px !important;
}

.el-select {
  width: 200px;
}
.el-slider {
  width: 400px;
}
.el-form-item{
  margin-bottom: 10px;
}
</style>
